/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    David Ungar, IBM Research - Initial Implementation
 *    Sam Adams, IBM Research - Initial Implementation
 *    Stefan Marr, Vrije Universiteit Brussel - Port to x86 Multi-Core Systems
 ******************************************************************************/


#include "headers.h"

void Squeak_Interpreter::enforced_pushReceiverVariableBytecode() {
  if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::ReadField_Of__Mask)) {
    omni_internal_read_field(roots.receiver, currentBytecode & 0xf);
  }
  else unenforced_pushReceiverVariableBytecode();
}

void Squeak_Interpreter::enforced_pushLiteralVariableBytecode() {
  if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::ReadLiteral__Mask))
    omni_internal_read_literal(currentBytecode & 0x1f);
  else unenforced_pushLiteralVariableBytecode();
}

void Squeak_Interpreter::enforced_storeAndPopReceiverVariableBytecode() {
  if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::WriteToFieldMask)) {
    Oop value  = internalStackTop();
    
    // REM: we might pop the Receiver (ReceiverIndex) here
    //      that should be ok, because, I hope, the next time we need it
    //      is when we return, and then we push it back
    //      and it is not deleted, just not protected by the stack
    //      pointer anymore.
    Oop newTop = internalStackValue(1);
    internalPop(2);
    
    omni_internal_write_field(roots.receiver, currentBytecode & 7, value, newTop);
  }
  else unenforced_storeAndPopReceiverVariableBytecode();
}

void Squeak_Interpreter::enforced_extendedPushBytecode() {
  u_char descriptor = fetchByte();
  fetchNextBytecode();
  int i = descriptor & 0x3f;
  switch ((descriptor >> 6) & 3) {
    case 0: {
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::ReadField_Of__Mask))
        omni_internal_read_field(roots.receiver, i);
      else
        pushReceiverVariable(i);
      break;
    }
    case 1: pushTemporaryVariable(i); break;
    case 2: pushLiteralConstant(i); break;
    case 3: {
      if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::ReadLiteral__Mask))
        omni_internal_read_literal(i);
      else
        pushLiteralVariable(i);
      break;
    }
  }
}

void Squeak_Interpreter::enforced_extendedStoreBytecode() {
  u_char d = fetchByte();
  
  u_char vi = d & 63;
  switch ((d >> 6) & 3) {
    case 0: {
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::WriteToFieldMask)) {
        Oop value = internalStackTop();
        internalPop(1);
        omni_internal_write_field(roots.receiver, vi, value);
      }
      else {
        fetchNextBytecode();
        // could watch for suspended context change here
        receiver_obj()->storePointer(vi, internalStackTop());
      }
      break;
    }
    case 1:
      fetchNextBytecode();
      localHomeContext()->storePointerIntoContext(
                                                  vi + Object_Indices::TempFrameStart, internalStackTop());
      break;
    case 2:
      fetchNextBytecode();
      fatal("illegal store");
    case 3: {
      if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::Write_ToLiteral__Mask)) {
        Oop val = internalStackTop();
        internalPop(1);
        omni_internal_write_literal(literal(vi), val);
      }
      else {
        fetchNextBytecode();
        literal(vi).as_object()->storePointer(Object_Indices::ValueIndex, internalStackTop());
      }
      break;
    }
  }
}

void Squeak_Interpreter::enforced_extendedStoreAndPopBytecode() {
  u_char d = fetchByte();
  
  u_char vi = d & 63;
  switch ((d >> 6) & 3) {
    case 0: {
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::WriteToFieldMask)) {
        Oop value  = internalStackTop();
        Oop newTop = internalStackValue(1);
        internalPop(2);
        omni_internal_write_field(roots.receiver, vi, value, newTop);
        return; 
      }
      else {
        fetchNextBytecode();
        // could watch for suspended context change here
        receiver_obj()->storePointer(vi, internalStackTop());
      }
      break;
    }
    case 1:
      fetchNextBytecode();
      localHomeContext()->storePointerIntoContext(
                                                  vi + Object_Indices::TempFrameStart, internalStackTop());
      break;
    case 2:
      fetchNextBytecode();
      fatal("illegal store");
    case 3: {
      if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::Write_ToLiteral__Mask)) {
        Oop val    = internalStackTop();
        Oop newTop = internalStackValue(1);        
        internalPop(2);
        omni_internal_write_literal(literal(vi), val, newTop);
        return;
      }
      else {
        fetchNextBytecode();
        literal(vi).as_object()->storePointer(Object_Indices::ValueIndex, internalStackTop());
      }
      break;
    }
  }
  internalPop(1);
}

void Squeak_Interpreter::enforced_singleExtendedSendBytecode() {
  // OMNI TODO: we need here also the usual omni_requires_delegation check, no?
  //            be careful, the use of perform:withArguments:inSuperclass: causes
  //            this bytecode to be used to, will probably end up in recursion
  //            we really need a clear meta-reflection handling :(
  
  u_char d = fetchByte();
  roots.messageSelector = literal(d & 0x1f);
  set_argumentCount( d >> 5 );
  
  Oop rcvr = internalStackValue(get_argumentCount());
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  if (delegate)
    omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_doubleExtendedDoAnythingBytecode() {
  /*
   "Replaces the Blue Book double-extended send [132], in which the first byte 
   was wasted on 8 bits of argument count.
   Here we use 3 bits for the operation sub-type (opType),  and the remaining
   5 bits for argument count where needed.
   The last byte give access to 256 instVars or literals.
   See also secondExtendedSendBytecode"
   */
  u_char b2 = fetchByte();
  u_char b3 = fetchByte();
  switch (b2 >> 5) {
    case 0: {
      roots.messageSelector = literal(b3);
      set_argumentCount( b2 & 31 );
      
      Oop rcvr = internalStackValue(get_argumentCount());
      bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
      if (delegate)
        omni_request_execution();
      
      enforced_normalSend();
      break;
    }
    case 1:
      roots.messageSelector = literal(b3);
      set_argumentCount( b2 & 31);
      enforced_superclassSend();
      break;
    case 2: {
      fetchNextBytecode();
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::ReadField_Of__Mask))
        omni_internal_read_field(roots.receiver, b3);
      else
        pushReceiverVariable(b3);
      break;
    }
    case 3:
      fetchNextBytecode();
      pushLiteralConstant(b3);
      break;
    case 4: {
      if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::ReadLiteral__Mask))
        omni_internal_read_literal(b3);
      else {
        fetchNextBytecode();
        pushLiteralVariable(b3);
      }
      break;
    }
    case 5:
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::WriteToFieldMask)) {
        Oop top = internalStackTop();
        internalPop(1);
        omni_internal_write_field(roots.receiver, b3, top);
      }
      else {
        fetchNextBytecode();
        // could watch for suspended context change here
        receiver_obj()->storePointer(b3, internalStackTop());
      }
      break;
    case 6: {
      if (omni_requires_intercession(roots.receiver, OstDomainSelector_Indices::WriteToFieldMask)) {
        Oop top    = internalStackTop();
        Oop newTop = internalStackValue(1);
        internalPop(2);
        omni_internal_write_field(roots.receiver, b3, top, newTop);
      }
      else {
        fetchNextBytecode();
        Oop top = internalStackTop();
        internalPop(1);
        // could watch for suspended context change here
        receiver_obj()->storePointer(b3, top);
      }
      break;
    }
    case 7: {
      if (omni_requires_delegation_for_literals(OstDomainSelector_Indices::Write_ToLiteral__Mask)) {
        Oop val = internalStackTop();
        internalPop(1);
        omni_internal_write_literal(literal(b3), val);
      }
      else {
        fetchNextBytecode();
        literal(b3).as_object()->storePointer(Object_Indices::ValueIndex, internalStackTop());
      }
      break;
    }
  }
}

void Squeak_Interpreter::enforced_secondExtendedSendBytecode() {
  /*
   This replaces the Blue Book double-extended super-send [134],
   which is subsumed by the new double-extended do-anything [132].
   It offers a 2-byte send of 0-3 args for up to 63 literals, for which
   the Blue Book opcode set requires a 3-byte instruction."
   */
  u_char descriptor = fetchByte();
  roots.messageSelector = literal(descriptor & 0x3f);
  set_argumentCount( descriptor >> 6 );
  assert (!internalStackValue(get_argumentCount()).is_mem()
          || The_Memory_System()->object_table->probably_contains((void*)internalStackValue(get_argumentCount()).bits()));
  
  Oop rcvr = internalStackValue(get_argumentCount());
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  if (delegate)
    omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_singleExtendedSuperBytecode() {
  u_char d = fetchByte();
  roots.messageSelector = literal(d & 0x1f);
  set_argumentCount( d >> 5 );
  enforced_superclassSend();
}

void Squeak_Interpreter::enforced_bytecodePrimAdd() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  
  bool delegate = false;  // OMNI: optimized the integer case by defering test
  
  if (areIntegers(rcvr, arg)) {
    oop_int_t r = rcvr.integerValue() + arg.integerValue();
    if (Oop::isIntegerValue(r)) {
      internalPopThenPush(2, Oop::from_int(r));
      fetchNextBytecode();
      return;
    }
  }
  else {
    delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
    
    if (!delegate) {
      successFlag = true;
      externalizeExecutionState();
      {
        Safepoint_Ability sa(true);
        primitiveFloatAdd(rcvr, arg);
      }
      internalizeExecutionState();
      if (successFlag) {
        fetchNextBytecode();
        return;
      }
    }
  }
  
  roots.messageSelector = specialSelector(0);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_shortConditionalJump()   { enforced_jumpIfFalseBy((currentBytecode & 7) + 1); }

void Squeak_Interpreter::enforced_longJumpIfTrue() {
  enforced_jumpIfTrueBy(long_cond_jump_offset());
}
void Squeak_Interpreter::enforced_longJumpIfFalse() {
  enforced_jumpIfFalseBy(long_cond_jump_offset());
}


void Squeak_Interpreter::enforced_bytecodePrimSubtract() {
  Oop rcvr = internalStackValue(1);
  
  bool delegate = false;  // OMNI: optimized the integer case by defering test
  
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    oop_int_t r = rcvr.integerValue() - arg.integerValue();
    if (Oop::isIntegerValue(r)) {
      internalPopThenPush(2, Oop::from_int(r));
      fetchNextBytecode();
      return;
    }
  }
  else {
    delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
    
    if (!delegate) {
      successFlag = true;
      externalizeExecutionState();
      {
        Safepoint_Ability sa(true);
        primitiveFloatSubtract(rcvr, arg);
      }
      internalizeExecutionState();
      if (successFlag) {
        fetchNextBytecode();
        return;
      }
    }
  }
  
  roots.messageSelector = specialSelector(1);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimMultiply() {
  Oop rcvr = internalStackValue(1);
  
  bool delegate = false;
  
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    oop_int_t ri = rcvr.integerValue();
    oop_int_t ai = arg.integerValue();
    long long result_with_overflow = (long long)ri * ai;
    if (Oop::isIntegerValue(result_with_overflow)) {
      internalPopThenPush(2, Oop::from_int(result_with_overflow));
      fetchNextBytecode();
      return;
    }
  }
  else {
    delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
    
    if (!delegate) {
      successFlag = true;
      externalizeExecutionState();
      {
        Safepoint_Ability sa(true);
        primitiveFloatMultiply(rcvr, arg);
      }
      internalizeExecutionState();
      if (successFlag) {
        fetchNextBytecode();
        return;
      }
    }
  }
  
  roots.messageSelector = specialSelector(8);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimDivide() {
  Oop rcvr = internalStackValue(1);
  
  bool delegate = false;
  
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    oop_int_t ri = rcvr.integerValue();
    oop_int_t ai = arg.integerValue();
    if (ai != 0   &&   ri % ai  == 0) {
      oop_int_t r = ri / ai;
      if (Oop::isIntegerValue(r)) {
        internalPopThenPush(2, Oop::from_int(r));
        fetchNextBytecode();
        return;
      }
    }
  }
  else {
    delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
    
    if (!delegate) {
      successFlag = true;
      externalizeExecutionState();
      {
        Safepoint_Ability sa(true);
        primitiveFloatDivide(rcvr, arg);
      }
      internalizeExecutionState();
      if (successFlag) {
        fetchNextBytecode();
        return;
      }
    }
  }
  
  roots.messageSelector = specialSelector(9);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimMod() {
  Oop rcvr = internalStackValue(1);
  
  successFlag = true;
  int mod = doPrimitiveMod(rcvr, internalStackValue(0));
  if (successFlag) {
    internalPopThenPush(2, Oop::from_int(mod));
    fetchNextBytecode();
    return;
  }
  
  roots.messageSelector = specialSelector(10);
  set_argumentCount(1);
  
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimLessThan() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() < arg.integerValue());
    return;
  }
  successFlag = true;
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  if (!delegate) {
    bool aBool = primitiveFloatLess(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(2);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimGreaterThan() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() > arg.integerValue());
    return;
  }
  
  successFlag = true;
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  if (!delegate) {
    bool aBool = primitiveFloatGreater(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(3);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimLessOrEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() <= arg.integerValue());
    return;
  }
  successFlag = true;
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  if (!delegate) {
    bool aBool = !primitiveFloatGreater(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(4);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimGreaterOrEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() >= arg.integerValue());
    return;
  }
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  if (!delegate) {
    successFlag = true;
    bool aBool = !primitiveFloatLess(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(5);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr == arg);
    return;
  }
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  if (!delegate) {
    successFlag = true;
    bool aBool = primitiveFloatEqual(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(6);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimNotEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr != arg);
    return;
  }
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  if (!delegate) {
    successFlag = true;
    bool aBool = !primitiveFloatEqual(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  
  roots.messageSelector = specialSelector(7);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimMakePoint() {
  // OMNI: this one is somehow special, only if it fails it is going to do
  //       a real message change, otherwise it is just going to store two
  //       references, or ints into a pointer object...
  //       What could be checked here is whether the point class allows
  //       sync exec, but I am not going to check that here...
  successFlag = true;
  externalizeExecutionState();
  {
    Safepoint_Ability sa(true);
    primitiveMakePoint();
  }
  internalizeExecutionState();
  if (successFlag) {
    fetchNextBytecode();
    return;
  }
  
  Oop rcvr = internalStackValue(1);
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask); 
  
  roots.messageSelector = specialSelector(11);
  set_argumentCount(1);
  
  if (delegate) omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimBitShift() {
  successFlag = true;
  externalizeExecutionState();
  {
    Safepoint_Ability sa(true);
    primitiveBitShift();
  }
  internalizeExecutionState();
  if (successFlag) {
    fetchNextBytecode();
    return;
  }
  
  roots.messageSelector = specialSelector(12);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimDiv() {
  successFlag = true;
  int32 quotient = doPrimitiveDiv(internalStackValue(1), internalStackValue(0));
  if (successFlag) {
    internalPopThenPush(2, Oop::from_int(quotient));
    fetchNextBytecode();
    return;
  }
  
  roots.messageSelector = specialSelector(13);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimBitAnd() {
  successFlag = true;
  externalizeExecutionState();
  {
    Safepoint_Ability sa(true);
    primitiveBitAnd();
  }
  internalizeExecutionState();
  if (successFlag) {
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(14);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}
void Squeak_Interpreter::enforced_bytecodePrimBitOr() {
  successFlag = true;
  externalizeExecutionState();
  {
    Safepoint_Ability sa(true);
    primitiveBitOr();
  }
  internalizeExecutionState();
  if (successFlag) {
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(15);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimAt() {
  Oop index = internalStackTop();
  Oop rcvr = internalStackValue(1);
  
  bool delegateExec = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  successFlag = rcvr.is_mem() && index.is_int();
  if (!delegateExec && successFlag) {
    At_Cache::Entry* e = atCache.get_entry(rcvr, false);
    if (e->matches(rcvr)) {
      // now, we are sure that it is a primitive, and we can delegate it directly to the domain
      bool delegatePrim = omni_requires_intercession(rcvr, OstDomainSelector_Indices::PrimAt_On__Mask);
      if (delegatePrim) {
        omni_internal_request_primitive_at(The_OstDomain.prim_at_on());
        return;
      }
      
      Oop result = commonVariableAt(rcvr, index.integerValue(), e, true);
      if (successFlag) {
        fetchNextBytecode();
        internalPopThenPush(2, result);
        return;
      }
    }
  }
  roots.messageSelector = specialSelector(16);
  set_argumentCount(1);
  
  if (delegateExec) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimAtPut() {
  Oop value = internalStackTop();
  Oop index = internalStackValue(1);
  Oop rcvr  = internalStackValue(2);
  
  bool delegateExec = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  successFlag = rcvr.is_mem() && index.is_int();
  if (!delegateExec && successFlag) {    
    At_Cache::Entry* e = atCache.get_entry(rcvr, true);
    if (e->matches(rcvr)) {
      bool delegatePrim = omni_requires_intercession(rcvr, OstDomainSelector_Indices::PrimAt_On_Put__Mask);
      if (delegatePrim) {
        omni_internal_request_primitive_atPut(The_OstDomain.prim_at_put_on());
        return;
      }
      
      commonVariableAtPut(rcvr, index.integerValue(), value, e);
      if (successFlag) {
        fetchNextBytecode();
        internalPopThenPush(3, value);
        return;
      }
    }
  }
  
  roots.messageSelector = specialSelector(17);
  set_argumentCount( 2 );
  
  if (delegateExec) omni_request_execution();
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimSize() {
  roots.messageSelector = specialSelector(18);
  set_argumentCount(0);
  
  Oop rcvr = internalStackTop();
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimNext() {
  roots.messageSelector = specialSelector(19);
  set_argumentCount(0);
  
  Oop rcvr = internalStackTop();
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimNextPut() {
  roots.messageSelector = specialSelector(20);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}


void Squeak_Interpreter::enforced_bytecodePrimAtEnd() {
  roots.messageSelector = specialSelector(21);
  set_argumentCount(0);
  
  Oop rcvr = internalStackTop();
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimClass() {  
  Oop rcvr = internalStackTop();
  
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask)) {
    set_argumentCount(0);
    roots.messageSelector = specialSelector(23);
    omni_request_execution();
    enforced_normalSend();
    return;
  }
  
  internalPopThenPush(1, rcvr.fetchClass());
  fetchNextBytecode();
}


void Squeak_Interpreter::enforced_bytecodePrimBlockCopy() {
  Oop rcvr = internalStackValue(1);
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  successFlag = true;
  success(rcvr.as_object()->hasContextHeader());
  if (!delegate && successFlag) {
    externalizeExecutionState();
    {
      Safepoint_Ability sa(true);
      primitiveBlockCopy();
    }
    internalizeExecutionState();
  }
  if (delegate || !successFlag) {
    roots.messageSelector = specialSelector(24);
    set_argumentCount(1);
    
    if (delegate) omni_request_execution();
    enforced_normalSend();
    return;
  }
  fetchNextBytecode();
}

void Squeak_Interpreter::enforced_commonBytecodePrimValue(int nargs, int selector_index) {
  Oop block = localSP()[-nargs];
  successFlag = true;
  set_argumentCount(nargs);
  
  // OMNI this looks like a slow operation, so try to fail fast
  //      usually we try to do the normal path first, like integer handling
  if (omni_requires_intercession(block, OstDomainSelector_Indices::RequestExecutionMask)) {
    roots.messageSelector = specialSelector(selector_index);
    omni_request_execution();
    enforced_normalSend();
    return;
  }
  
  
  Oop klass = block.fetchClass();
  bool classOK = true;
  
# if Include_Closure_Support
  if (klass == splObj(Special_Indices::ClassBlockClosure)) {
    externalizeExecutionState();
    primitiveClosureValue();
    internalizeExecutionState();
  }
  else 
# endif
    if (klass == splObj(Special_Indices::ClassBlockContext)) {
      externalizeExecutionState();
      primitiveValue();
      internalizeExecutionState();
    } 
    else
      classOK = false;
  
  if (classOK && successFlag) 
    fetchNextBytecode();
  else {
    roots.messageSelector = specialSelector(selector_index);
    enforced_normalSend();
  }  
}

void Squeak_Interpreter::enforced_bytecodePrimValue() {
  enforced_commonBytecodePrimValue(0, 25);
}

void Squeak_Interpreter::enforced_bytecodePrimValueWithArg() {
  enforced_commonBytecodePrimValue(1, 26);
}

void Squeak_Interpreter::enforced_bytecodePrimDo() {
  roots.messageSelector = specialSelector(27);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}
void Squeak_Interpreter::enforced_bytecodePrimNew() {
  roots.messageSelector = specialSelector(28);
  set_argumentCount(0);
  
  Oop rcvr = internalStackTop();
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}
void Squeak_Interpreter::enforced_bytecodePrimNewWithArg() {
  roots.messageSelector = specialSelector(29);
  set_argumentCount(1);
  
  Oop rcvr = internalStackValue(1);
  if (omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask))
    omni_request_execution();
  
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimPointX() {
  successFlag = true;
  Oop rcvr = internalStackTop();
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask); 
  
  if (!delegate) {
    assertClass(rcvr, splObj(Special_Indices::ClassPoint));
    if (successFlag) {
      internalPopThenPush(1, rcvr.as_object()->fetchPointer(Object_Indices::XIndex));
      fetchNextBytecode();
      return;
    }
  }
  
  roots.messageSelector = specialSelector(30);
  set_argumentCount(0);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_bytecodePrimPointY() {
  successFlag = true;
  Oop rcvr = internalStackTop();
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  
  assertClass(rcvr, splObj(Special_Indices::ClassPoint));
  if (successFlag) {
    internalPopThenPush(1, rcvr.as_object()->fetchPointer(Object_Indices::YIndex));
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(31);
  set_argumentCount(0);
  
  if (delegate) omni_request_execution();
  enforced_normalSend();
}

void Squeak_Interpreter::enforced_sendLiteralSelectorBytecode() {
	// "Can use any of the first 16 literals for the selector and pass up to 2 arguments."
  assert(method_obj()->isCompiledMethod());
	roots.messageSelector = literal(currentBytecode & 0xf);
  
  if (check_assertions && !roots.messageSelector.is_mem()) {
    Printer* p = error_printer;
    p->printf("on %d: msgSel is int; method bits 0x%x, method->obj 0x%x, method obj 0x%x, method obj as_oop 0x%x, msgSel 0x%x\n",
              Logical_Core::my_rank(), method().bits(), (Object*)method().as_object(), (Object*)method_obj(), method_obj()->as_oop().bits(), roots.messageSelector.bits());
    method_obj()->print(p);
    p->nl();
    method_obj()->print_compiled_method(p);
    p->nl();
    
    OS_Interface::abort();
  }
  assert(roots.messageSelector.is_mem());
  
  int arg_count = ((currentBytecode >> 4) & 3) - 1 ;
	set_argumentCount(arg_count);
  
  Oop rcvr = internalStackValue(arg_count);
  
  bool delegate = omni_requires_intercession(rcvr, OstDomainSelector_Indices::RequestExecutionMask);
  if (delegate)
    omni_request_execution();
  enforced_normalSend();
}


