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

void Squeak_Interpreter::unenforced_pushReceiverVariableBytecode() {
  fetchNextBytecode();
  pushReceiverVariable(prevBytecode & 0xf);
}

void Squeak_Interpreter::pushTemporaryVariableBytecode() {
  fetchNextBytecode();
  pushTemporaryVariable(prevBytecode & 0xf);
}
void Squeak_Interpreter::pushLiteralConstantBytecode() {
  fetchNextBytecode();
  pushLiteralConstant(prevBytecode & 0x1f);
}

void Squeak_Interpreter::unenforced_pushLiteralVariableBytecode() {
  fetchNextBytecode();
  pushLiteralVariable(prevBytecode & 0x1f);
}


void Squeak_Interpreter::unenforced_storeAndPopReceiverVariableBytecode() {
  fetchNextBytecode();
  // could watch for suspended context change here
  receiver_obj()->storePointer(prevBytecode & 7, internalStackTop());
  internalPop(1);
}

void Squeak_Interpreter::storeAndPopTemporaryVariableBytecode() {
  fetchNextBytecode();
  assert(_localHomeContext != roots.nilObj.as_object());
	localHomeContext()->storePointerIntoContext((prevBytecode & 7) + Object_Indices::TempFrameStart, internalStackTop());
	internalPop(1);
}
void Squeak_Interpreter::pushReceiverBytecode() {
  fetchNextBytecode();
  internalPush(roots.receiver);
}
void Squeak_Interpreter::pushConstantTrueBytecode() {
  fetchNextBytecode();
  internalPush(roots.trueObj);
}
void Squeak_Interpreter::pushConstantFalseBytecode() {
  fetchNextBytecode();
  internalPush(roots.falseObj);
}
void Squeak_Interpreter::pushConstantNilBytecode() {
  fetchNextBytecode();
  internalPush(roots.nilObj);
}
void Squeak_Interpreter::pushConstantMinusOneBytecode() {
  fetchNextBytecode();
  internalPush(Oop::from_int(-1));
}
void Squeak_Interpreter::pushConstantZeroBytecode() {
  fetchNextBytecode();
  internalPush(Oop::from_int(0));
}
void Squeak_Interpreter::pushConstantOneBytecode() {
  fetchNextBytecode();
  internalPush(Oop::from_int(1));
}
void Squeak_Interpreter::pushConstantTwoBytecode() {
  fetchNextBytecode();
  internalPush(Oop::from_int(2));
}

void Squeak_Interpreter::returnReceiver() {
  commonReturn(sender(), roots.receiver);
}
void Squeak_Interpreter::returnTrue() {
  commonReturn(sender(), roots.trueObj);
}
void Squeak_Interpreter::returnFalse() {
  commonReturn(sender(), roots.falseObj);
}
void Squeak_Interpreter::returnNil() {
  commonReturn(sender(), roots.nilObj);
}

void Squeak_Interpreter::returnTopFromMethod() {
  commonReturn(sender(), internalStackTop());
}
void Squeak_Interpreter::returnTopFromBlock() {
  commonReturn(caller(), internalStackTop());
}


void Squeak_Interpreter::unknownBytecode() {
  untested();
  fatal("unknown bytecode");
}


void Squeak_Interpreter::unenforced_extendedPushBytecode() {
  u_char descriptor = fetchByte();
  fetchNextBytecode();
  int i = descriptor & 0x3f;
  switch ((descriptor >> 6) & 3) {
    case 0: pushReceiverVariable(i);  break;
    case 1: pushTemporaryVariable(i); break;
    case 2: pushLiteralConstant(i); break;
    case 3: pushLiteralVariable(i); break;
  }
}

void Squeak_Interpreter::unenforced_extendedStoreBytecode() {
  u_char d = fetchByte();
  fetchNextBytecode();
  u_char vi = d & 63;
  switch ((d >> 6) & 3) {
    case 0:
      // could watch for suspended context change here
      receiver_obj()->storePointer(vi, internalStackTop());
      break;
    case 1:
      localHomeContext()->storePointerIntoContext(
                             vi + Object_Indices::TempFrameStart, internalStackTop());
      break;
    case 2:
      fatal("illegal store");
    case 3:
      literal(vi).as_object()->storePointer(Object_Indices::ValueIndex, internalStackTop());
      break;
  }
}

void Squeak_Interpreter::unenforced_extendedStoreAndPopBytecode() {
  unenforced_extendedStoreBytecode();
  internalPop(1);
}

void Squeak_Interpreter::unenforced_singleExtendedSendBytecode() {
  u_char d = fetchByte();
  roots.messageSelector = literal(d & 0x1f);
  set_argumentCount( d >> 5 );
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_doubleExtendedDoAnythingBytecode() {
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
    case 0:
      roots.messageSelector = literal(b3);
      set_argumentCount( b2 & 31 );      
      unenforced_normalSend();
      break;
    case 1:
      roots.messageSelector = literal(b3);
      set_argumentCount( b2 & 31);
      unenforced_superclassSend();
      break;
    case 2:
      fetchNextBytecode();
      pushReceiverVariable(b3);
      break;
    case 3:
      fetchNextBytecode();
      pushLiteralConstant(b3);
      break;
    case 4:
      fetchNextBytecode();
      pushLiteralVariable(b3);
      break;
    case 5:
      fetchNextBytecode();
      // could watch for suspended context change here
      receiver_obj()->storePointer(b3, internalStackTop());
      break;
    case 6: {
      fetchNextBytecode();
      Oop top = internalStackTop();
      internalPop(1);
      // could watch for suspended context change here
      receiver_obj()->storePointer(b3, top);
      break;
    }
    case 7:
      fetchNextBytecode();
      literal(b3).as_object()->storePointer(Object_Indices::ValueIndex, internalStackTop());
      break;
  }
}

void Squeak_Interpreter::unenforced_singleExtendedSuperBytecode() {
  u_char d = fetchByte();
  roots.messageSelector = literal(d & 0x1f);
  set_argumentCount( d >> 5 );
  unenforced_superclassSend();
}

void Squeak_Interpreter::unenforced_secondExtendedSendBytecode() {
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
  unenforced_normalSend();
}

void Squeak_Interpreter::popStackBytecode() { fetchNextBytecode(); internalPop(1); }

void Squeak_Interpreter::duplicateTopBytecode() {
  fetchNextBytecode();
  internalPush(internalStackTop());
}
void Squeak_Interpreter::pushActiveContextBytecode() {
  fetchNextBytecode();
  reclaimableContextCount = 0;
  internalPush(activeContext());
}
void Squeak_Interpreter::experimentalBytecode() {
  untested();
  unimplemented();
}
void Squeak_Interpreter::shortUnconditionalJump() { jump((currentBytecode & 7) + 1); }
void Squeak_Interpreter::unenforced_shortConditionalJump()   { unenforced_jumpIfFalseBy((currentBytecode & 7) + 1); }


void Squeak_Interpreter::longUnconditionalJump() {
  int offset = long_jump_offset();
  set_localIP(localIP() + offset);
  if (offset < 0)
    internalQuickCheckForInterrupts();
  fetchNextBytecode();
}
void Squeak_Interpreter::unenforced_longJumpIfTrue() {
  unenforced_jumpIfTrueBy(long_cond_jump_offset());
}
void Squeak_Interpreter::unenforced_longJumpIfFalse() {
  unenforced_jumpIfFalseBy(long_cond_jump_offset());
}


void Squeak_Interpreter::omni_request_execution() {
  /*** STEFAN TODO: Check whether we need a specific safepoint ability here.
                    Similar to the DNU or ensemble msg send? */
  
  /* requestExecution.... of: aSelector on: aRcvr */
  Oop rcvr = internalStackValue(get_argumentCount());
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  int argCnt = get_argumentCount();
  
  assert(rcvr        != Oop::from_bits(0));
  assert(rcvr_domain != Oop::from_bits(0));
  
  /* set domain as receiver */
  DEBUG_STORE_CHECK(&localSP()[-argCnt], rcvr_domain);
  localSP()[-argCnt] = rcvr_domain;
  
  /* push the rest on the stack */
  internalPush(roots.messageSelector);
  internalPush(rcvr);
  
  set_argumentCount(argCnt + 2);

  // Now change the selector
  roots.messageSelector = The_OstDomain.request_exec(argCnt);
}

void Squeak_Interpreter::omni_request_execution_in_lookup_class(Oop lkupClass) {
  pushRemappableOop(lkupClass); // make GC safe
  
  /* requestExecutionOf: aSelector with: argArray lookup: cls on: obj */
  
  Object_p argumentArray_obj = splObj_obj(Special_Indices::ClassArray)->instantiateClass(get_argumentCount());
  
  lkupClass = popRemappableOop(); // remap after GC
  
  Oop rcvr = internalStackValue(get_argumentCount());
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  int original_arg_cnt = get_argumentCount();

  // fill array with arguments
  oopcpy_no_store_check(argumentArray_obj->as_oop_p() + sizeof(argumentArray_obj->baseHeader)/sizeof(Oop),
                        localSP() - (original_arg_cnt - 1),
                        original_arg_cnt,
                        argumentArray_obj);
	argumentArray_obj->beRootIfOld();
  
  /* set domain as receiver */
  DEBUG_STORE_CHECK(&localSP()[-original_arg_cnt], rcvr_domain);
  localSP()[-original_arg_cnt] = rcvr_domain;
  
  /* push all the arguments */
  internalPopThenPush(original_arg_cnt, roots.messageSelector);
  internalPush(argumentArray_obj->as_oop());
  internalPush(lkupClass);
  internalPush(rcvr);
  
  set_argumentCount(4);
  
  roots.messageSelector = The_OstDomain.request_exec_in_lookup_class();
  roots.lkupClass       = rcvr_domain.fetchClass();
  roots.receiverClass   = roots.lkupClass;
}

void Squeak_Interpreter::omni_request_primitive_at(Oop primSelector) {
  // this is code that should be called from a primitive only, and primitives are supposed to work on the externalized state
  Oop index  = stackTop();
  Oop rcvr   = stackValue(1);
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  popThenPush(2, rcvr_domain);
  push(index);
  push(rcvr);
  
  set_argumentCount(2);
  roots.messageSelector = primSelector;
  roots.lkupClass = rcvr_domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);  // rcvr_domain.fetchClass()
}

void Squeak_Interpreter::omni_internal_request_primitive_at(Oop primSelector) {
  // this is code that should be called from a primitive only, and primitives are supposed to work on the externalized state
  Oop index  = internalStackTop();
  Oop rcvr   = internalStackValue(1);
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  internalPopThenPush(2, rcvr_domain);
  internalPush(index);
  internalPush(rcvr);
  
  set_argumentCount(2);
  roots.messageSelector = primSelector;
  roots.lkupClass = rcvr_domain.fetchClass();
  
  omni_commonInternalSend();
}


void Squeak_Interpreter::omni_request_primitive_atPut(Oop primSelector) {
  Oop value = stackTop();
  Oop index = stackValue(1);
  Oop rcvr  = stackValue(2);
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  popThenPush(3, rcvr_domain);
  push(index);
  push(rcvr);
  push(value);
  
  set_argumentCount(3);
  roots.messageSelector = primSelector;
  roots.lkupClass = rcvr_domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);
}

void Squeak_Interpreter::omni_internal_request_primitive_atPut(Oop primSelector) {
  Oop value = internalStackTop();
  Oop index = internalStackValue(1);
  Oop rcvr  = internalStackValue(2);
  Oop rcvr_domain = rcvr.as_object()->domain_oop();
  
  internalPopThenPush(3, rcvr_domain);
  internalPush(index);
  internalPush(rcvr);
  internalPush(value);
  
  set_argumentCount(3);
  roots.messageSelector = primSelector;
  roots.lkupClass = rcvr_domain.fetchClass();
  
  omni_commonInternalSend();
}

void Squeak_Interpreter::omni_request_primitive_clone() {
  Oop value = stackTop();
  
  Oop domain = value.as_object()->domain_oop();
  
  popThenPush(1, domain);
  push(value);
  
  set_argumentCount(1);
  
  roots.messageSelector = The_OstDomain.prim_shallow_copy();
  roots.lkupClass = domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);
}


void Squeak_Interpreter::omni_request_primitive_next() {
  Oop rcvr = stackTop();
  
  Oop domain = rcvr.as_object()->domain_oop();
  
  popThenPush(1, domain);
  push(rcvr);
  
  set_argumentCount(1);
  
  roots.messageSelector = The_OstDomain.prim_next();
  roots.lkupClass       = domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);
}


void Squeak_Interpreter::omni_request_primitive_nextPut() {
  Oop val  = stackTop();
  Oop rcvr = stackValue(1);
  
  Oop domain = rcvr.as_object()->domain_oop();
  
  popThenPush(2, domain);
  push(rcvr);
  push(val);
  
  set_argumentCount(2);
  
  roots.messageSelector = The_OstDomain.prim_next_put();
  roots.lkupClass       = domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);
}


void Squeak_Interpreter::omni_request_primitive_replaceFromToWithStartingAt() {
  Oop rcvr  = stackValue(4);
  
  Oop start = stackValue(3);
  Oop stop  = stackValue(2);
  Oop repl  = stackValue(1);
  Oop replStart = stackValue(0);

  Oop domain = rcvr.as_object()->domain_oop();
  
  popThenPush(5, domain);
  
  push(start);
  push(stop);
  push(repl);
  push(replStart);
  push(rcvr);
  
  set_argumentCount(5);
  
  roots.messageSelector = The_OstDomain.prim_replace_from_to_with_starting_at_on();
  roots.lkupClass       = domain.fetchClass();
  
  omni_commonSend(roots.lkupClass);
}


void Squeak_Interpreter::omni_read_field(Oop obj_oop, int idx) {
  Safepoint_Ability sa(true);

  Object_p obj    = obj_oop.as_object();
  Oop domain      = obj->domain_oop();
  Oop lookupClass = domain.fetchClass();
  
  /* readField: idx of: obj */
  
  
  assert(obj_oop != Oop::from_bits(0));
  assert(domain  != Oop::from_bits(0));

  // we assume that at this point obj_oop was already popped from the stack
  push(domain);

  pushInteger(idx + 1);  // Moving that up to Smalltalk means a conversion to 1-based indexing
  push(obj_oop);
    
  set_argumentCount(2);
  roots.messageSelector = The_OstDomain.read_field();
  
  omni_commonSend(lookupClass);
}

void Squeak_Interpreter::omni_commonSend(Oop lookupClass) {
  findNewMethodInClass(lookupClass);
  
  {
    Object_p nmo = newMethod_obj();
    if (nmo->isCompiledMethod())
      success(nmo->argumentCount() == get_argumentCount());
  }
  
  if (successFlag) {
    executeNewMethodFromCache();
    successFlag = true;
  }
  else
    fatal("not yet implemented");
}

void Squeak_Interpreter::omni_commonInternalSend() {
  Safepoint_Ability sa(false);
  internalFindNewMethod();
  unenforced_internalExecuteNewMethod();
  
  if (process_is_scheduled_and_executing()) // xxxxxxx predicate only needed to satisfy assertions?
    fetchNextBytecode();
}

/** STEFAN: make sure this is in sync with the normal read_field */
void Squeak_Interpreter::omni_internal_read_field(Oop obj_oop, int idx) {
  Safepoint_Ability sa(false);
  
  Object_p obj = obj_oop.as_object();
  Oop domain = obj->domain_oop();
  

  /* readField: idx of: obj */
  
  
  assert(obj_oop != Oop::from_bits(0));
  assert(domain  != Oop::from_bits(0));
  
  // we assume that at this point obj_oop was already popped from the stack
  internalPush(domain);
  
  internalPush(Oop::from_int(idx + 1));  // Moving that up to Smalltalk means a conversion to 1-based indexing
  internalPush(obj_oop);
  
  set_argumentCount(2);
  
  roots.receiverClass   = roots.lkupClass = domain.fetchClass();
  roots.messageSelector = The_OstDomain.read_field();
  
  omni_commonInternalSend();
}



void Squeak_Interpreter::omni_write_field(Oop obj_oop, int idx, Oop value) {
  Safepoint_Ability sa(true);
  
  Object_p obj    = obj_oop.as_object();
  Oop domain      = obj->domain_oop();
  Oop lookupClass = domain.fetchClass();
  
  /* write: val toField: idx of: obj */
  
  push(domain);
  push(value);
  pushInteger(idx + 1);  // Moving that up to Smalltalk means a conversion to 1-based indexing
  push(obj_oop);
    
  set_argumentCount(3);
  
  roots.messageSelector = The_OstDomain.write_field();
  
  omni_commonSend(lookupClass);
}

/** STEFAN: make sure this is in sync with the normal write_field */
void Squeak_Interpreter::omni_internal_write_field(Oop obj_oop, int idx, Oop value) {
  Safepoint_Ability sa(false);
  
  Object_p obj = obj_oop.as_object();
  Oop domain   = obj->domain_oop();
  
  /* write: val toField: idx of: obj */
  
  
  assert(obj_oop != Oop::from_bits(0));
  assert(domain  != Oop::from_bits(0));
  
  // we assume that at this point obj_oop was already popped from the stack
  internalPush(domain);
  internalPush(value);
  internalPush(Oop::from_int(idx + 1));  // Moving that up to Smalltalk means a conversion to 1-based indexing
  internalPush(obj_oop);
  
  set_argumentCount(3);
  
  roots.receiverClass   = roots.lkupClass = domain.fetchClass();
  roots.messageSelector = The_OstDomain.write_field();
  
  omni_commonInternalSend();
}

void Squeak_Interpreter::omni_internal_write_field(Oop obj_oop, int idx, Oop value, Oop newTop) {
  Safepoint_Ability sa(false);
  
  Object_p obj = obj_oop.as_object();
  Oop domain   = obj->domain_oop();

  assert(obj_oop != Oop::from_bits(0));
  assert(domain  != Oop::from_bits(0));
  
  // current frame is already prepared for being only used when the result
  // got pushed back on (we know the result already: newTop)
  // will now use a special send, that is not modifying the current frame
  // anymore

  // issue direct send:
  set_argumentCount(4);
  roots.receiverClass   = roots.lkupClass       = domain.fetchClass();
  roots.messageSelector = The_OstDomain.write_field_with_return();
  omni_internal_send_create_context_directly(domain, value, Oop::from_int(idx + 1), obj_oop, newTop);
}

void Squeak_Interpreter::omni_internal_send_create_context_directly(Oop receiver, Oop arg1, Oop arg2, Oop arg3) {
  debugCommonSend();
  
  // since we provide the arguments explicitly to construct the context,
  // we don't support #dnu and #cannotInterpret:
  // just as a reminder, would need to check the whole hierarchy to be sure
  assert(roots.nilObj != roots.lkupClass.as_object()->fetchPointer(Object_Indices::MessageDictionaryIndex));
  internalFindNewMethod();
  
  // we also do not support primitives here
  assert_eq(primitiveIndex, 0, "Primitives are not supported when send is done with direct context construction.");
  
  internal_activate_with_arguments(receiver, arg1, arg2, arg3);
  
  if (process_is_scheduled_and_executing()) // xxxxxxx predicate only needed to satisfy assertions?
    fetchNextBytecode();
}

void Squeak_Interpreter::omni_internal_send_create_context_directly(Oop receiver, Oop arg1, Oop arg2, Oop arg3, Oop arg4) {
  debugCommonSend();
  
  // since we provide the arguments explicitly to construct the context,
  // we don't support #dnu and #cannotInterpret:
  // just as a reminder, would need to check the whole hierarchy to be sure
  assert(roots.nilObj != roots.lkupClass.as_object()->fetchPointer(Object_Indices::MessageDictionaryIndex));
  internalFindNewMethod();
  
  // we also do not support primitives here
  assert_eq(primitiveIndex, 0, "Primitives are not supported when send is done with direct context construction.");
  
  internal_activate_with_arguments(receiver, arg1, arg2, arg3, arg4);
  
  if (process_is_scheduled_and_executing()) // xxxxxxx predicate only needed to satisfy assertions?
    fetchNextBytecode();
}

void Squeak_Interpreter::omni_internal_read_literal(oop_int_t idx) {
  Safepoint_Ability sa(false);
  
  Oop lit = literal(idx);
  Oop domain = _localDomain->as_oop();
  
  /* readLiteral: literal */
  
  internalPush(domain);
  internalPush(lit);
  
  set_argumentCount(1);
  
  roots.lkupClass = domain.fetchClass();
  roots.messageSelector = The_OstDomain.read_literal();
  
  omni_commonInternalSend();
}


void Squeak_Interpreter::omni_internal_write_literal(Oop lit, Oop value) {
  Safepoint_Ability sa(false);
  
  Oop domain   = _localDomain->as_oop();
  
  /* write: val toLiteral: obj */
  
  // we assume that at this point 'value' was already popped from the stack
  internalPush(domain);
  internalPush(value);
  internalPush(lit);
  
  set_argumentCount(2);
  
  roots.lkupClass = domain.fetchClass();
  roots.messageSelector = The_OstDomain.write_literal();
  
  omni_commonInternalSend();
}


void Squeak_Interpreter::omni_internal_write_literal(Oop lit, Oop value, Oop newTop) {
  Safepoint_Ability sa(false);
  
  Oop domain   = _localDomain->as_oop();
  
  // current frame is already prepared for being only used when the result
  // got pushed back on (we know the result already: newTop)
  // will now use a special send, that is not modifying the current frame
  // anymore
  
  // issue direct send:
  set_argumentCount(3);
  
  roots.lkupClass       = domain.fetchClass();
  roots.messageSelector = The_OstDomain.write_literal_with_return();
  omni_internal_send_create_context_directly(domain, value, lit, newTop);
}


void Squeak_Interpreter::unenforced_bytecodePrimAdd() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);

  if (areIntegers(rcvr, arg)) {
    oop_int_t r = rcvr.integerValue() + arg.integerValue();
    if (Oop::isIntegerValue(r)) {
      internalPopThenPush(2, Oop::from_int(r));
      fetchNextBytecode();
      return;
    }
  }
  else {
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
  roots.messageSelector = specialSelector(0);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimSubtract() {
  Oop rcvr = internalStackValue(1);
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
  roots.messageSelector = specialSelector(1);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimMultiply() {
  Oop rcvr = internalStackValue(1);
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
  roots.messageSelector = specialSelector(8);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimDivide() {
  Oop rcvr = internalStackValue(1);
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
  roots.messageSelector = specialSelector(9);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimMod() {
  successFlag = true;
  int mod = doPrimitiveMod(internalStackValue(1), internalStackValue(0));
  if (successFlag) {
    internalPopThenPush(2, Oop::from_int(mod));
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(10);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimLessThan() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() < arg.integerValue());
    return;
  }
  else {
    successFlag = true;
    bool aBool = primitiveFloatLess(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(2);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimGreaterThan() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() > arg.integerValue());
    return;
  }
  else {
    successFlag = true;
    bool aBool = primitiveFloatGreater(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(3);
  set_argumentCount(1);
  unenforced_normalSend();
}


void Squeak_Interpreter::unenforced_bytecodePrimLessOrEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() <= arg.integerValue());
    return;
  }
  else {  
	successFlag = true;
    bool aBool = !primitiveFloatGreater(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(4);
  set_argumentCount(1);
  unenforced_normalSend();
}


void Squeak_Interpreter::unenforced_bytecodePrimGreaterOrEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr.integerValue() >= arg.integerValue());
    return;
  }
  else {
    successFlag = true;
    bool aBool = !primitiveFloatLess(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(5);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr == arg);
    return;
  }
  else {
    successFlag = true;
    bool aBool = primitiveFloatEqual(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(6);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimNotEqual() {
  Oop rcvr = internalStackValue(1);
  Oop arg  = internalStackValue(0);
  if (areIntegers(rcvr, arg)) {
    booleanCheat(rcvr != arg);
    return;
  }
  else {
    successFlag = true;
    bool aBool = !primitiveFloatEqual(rcvr, arg);
    if (successFlag) {
      booleanCheat(aBool);
      return;
    }
  }
  roots.messageSelector = specialSelector(7);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimMakePoint() {
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
  roots.messageSelector = specialSelector(11);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimBitShift() {
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
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimDiv() {
  successFlag = true;
  int32 quotient = doPrimitiveDiv(internalStackValue(1), internalStackValue(0));
  if (successFlag) {
    internalPopThenPush(2, Oop::from_int(quotient));
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(13);
  set_argumentCount(1);
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimBitAnd() {
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
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimBitOr() {
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
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimAt() {
  Oop index = internalStackTop();
  Oop rcvr = internalStackValue(1);
  successFlag = rcvr.is_mem() && index.is_int();
  if (successFlag) {
    At_Cache::Entry* e = atCache.get_entry(rcvr, false);
    if (e->matches(rcvr)) {
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
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimAtPut() {
  Oop value = internalStackTop();
  Oop index = internalStackValue(1);
  Oop rcvr  = internalStackValue(2);
  successFlag = rcvr.is_mem() && index.is_int();
  if (successFlag) {
    At_Cache::Entry* e = atCache.get_entry(rcvr, true);
    if (e->matches(rcvr)) {
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
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimSize() {
  roots.messageSelector = specialSelector(18);
  set_argumentCount(0);
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimNext() {
  roots.messageSelector = specialSelector(19);
  set_argumentCount(0);
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimNextPut() {
  roots.messageSelector = specialSelector(20);
  set_argumentCount(1);
  unenforced_normalSend();
}


void Squeak_Interpreter::unenforced_bytecodePrimAtEnd() {
  roots.messageSelector = specialSelector(21);
  set_argumentCount(0);
  unenforced_normalSend();
}
void Squeak_Interpreter::bytecodePrimEquivalent() {
  booleanCheat(internalStackValue(1) == internalStackValue(0));
}

void Squeak_Interpreter::unenforced_bytecodePrimClass() {
  internalPopThenPush(1, internalStackTop().fetchClass());
  fetchNextBytecode();
}

void Squeak_Interpreter::unenforced_bytecodePrimBlockCopy() {
  Oop rcvr = internalStackValue(1);
  successFlag = true;
  success(rcvr.as_object()->hasContextHeader());
  if (successFlag) {
    externalizeExecutionState();
    {
      Safepoint_Ability sa(true);
      primitiveBlockCopy();
    }
    internalizeExecutionState();
  }
  if (!successFlag) {
    roots.messageSelector = specialSelector(24);
    set_argumentCount(1);
    unenforced_normalSend();
    return;
  }
  fetchNextBytecode();
}


void Squeak_Interpreter::unenforced_commonBytecodePrimValue(int nargs, int selector_index) {
  Oop block = localSP()[-nargs];
  successFlag = true;
  set_argumentCount(nargs);
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
    unenforced_normalSend();
  }  
}

void Squeak_Interpreter::unenforced_bytecodePrimValue() {
  unenforced_commonBytecodePrimValue(0, 25);
}

void Squeak_Interpreter::unenforced_bytecodePrimValueWithArg() {
  unenforced_commonBytecodePrimValue(1, 26);
}

void Squeak_Interpreter::unenforced_bytecodePrimDo() {
  roots.messageSelector = specialSelector(27);
  set_argumentCount(1);
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimNew() {
  roots.messageSelector = specialSelector(28);
  set_argumentCount(0);
  unenforced_normalSend();
}
void Squeak_Interpreter::unenforced_bytecodePrimNewWithArg() {
  roots.messageSelector = specialSelector(29);
  set_argumentCount(1);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimPointX() {
  successFlag = true;
  Oop rcvr = internalStackTop();
  assertClass(rcvr, splObj(Special_Indices::ClassPoint));
  if (successFlag) {
    internalPopThenPush(1, rcvr.as_object()->fetchPointer(Object_Indices::XIndex));
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(30);
  set_argumentCount(0);
  unenforced_normalSend();
}

void Squeak_Interpreter::unenforced_bytecodePrimPointY() {
  successFlag = true;
  Oop rcvr = internalStackTop();
  assertClass(rcvr, splObj(Special_Indices::ClassPoint));
  if (successFlag) {
    internalPopThenPush(1, rcvr.as_object()->fetchPointer(Object_Indices::YIndex));
    fetchNextBytecode();
    return;
  }
  roots.messageSelector = specialSelector(31);
  set_argumentCount(0);
  unenforced_normalSend();
}


void Squeak_Interpreter::unenforced_sendLiteralSelectorBytecode() {
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
  set_argumentCount(((currentBytecode >> 4) & 3) - 1);
  unenforced_normalSend();
}

# if Include_Closure_Support

void Squeak_Interpreter::pushNewArrayBytecode() {
  u_char size = fetchByte();
  bool popValues = size > 127;
  size &= 127;
  fetchNextBytecode();
  externalizeExecutionState();
  Object_p array_obj;
  {
    Safepoint_Ability sa(true);
    array_obj = splObj_obj(Special_Indices::ClassArray)->instantiateClass(size);
  }
  internalizeExecutionState();
  if (popValues) {
    for ( int i = 0;  i < size;  ++i )
      // Assume new Array is young, so use unchecked stores
      array_obj->storePointerUnchecked(i, internalStackValue(size - i - 1));
    internalPop(size);
  }
  
  Oop newArray = array_obj->as_oop();
  
  omni_set_domain_for_new_object(array_obj);
  
  internalPush(newArray);
}


void Squeak_Interpreter::pushRemoteTempLongBytecode() {
  u_char remoteTempIndex = fetchByte();
  u_char tempVectorIndex = fetchByte();
  fetchNextBytecode();
  pushRemoteTempInVectorAt(remoteTempIndex, tempVectorIndex);
}

void Squeak_Interpreter::storeRemoteTempLongBytecode() {
  u_char remoteTempIndex = fetchByte(); // which temp on stack
  u_char tempVectorIndex = fetchByte(); // which 0-origin index into vector
  fetchNextBytecode();
  storeRemoteTempInVectorAt(remoteTempIndex, tempVectorIndex);
}

void Squeak_Interpreter::storeAndPopRemoteTempLongBytecode() {
  storeRemoteTempLongBytecode();
  internalPop(1);
}


void Squeak_Interpreter::pushRemoteTempInVectorAt(u_char indexIntoVector, u_char  indexOfVectorIntoContext) {
  Oop tempVector = temporary(indexOfVectorIntoContext);
  internalPush(tempVector.as_object()->fetchPointer(indexIntoVector));
}

void Squeak_Interpreter::storeRemoteTempInVectorAt(u_char indexIntoVector, u_char  indexOfVectorIntoContext) {
  Oop tempVector = temporary(indexOfVectorIntoContext);
  tempVector.as_object()->storePointer(indexIntoVector, internalStackTop());
}


void Squeak_Interpreter::pushClosureCopyCopiedValuesBytecode() {
  /* "The compiler has pushed the values to be copied, if any.  Find numArgs 
      and numCopied in the byte following.
      Create a Closure with space for the copiedValues and pop numCopied
      values off the stack into the closure.
      Set numArgs as specified, and set startpc to the pc following the block
      size and jump over that code."*/
  
  image_version = Squeak_Image_Reader::Post_Closure_32_Bit_Image_Version;

  u_char numArgsNumCopied = fetchByte();
  u_int32 numArgs   = numArgsNumCopied & 0xf;
  u_int32 numCopied = numArgsNumCopied >> (u_int32)4;
  
  u_int32 blockSize = fetchByte() << 8;
  blockSize += (u_int32)fetchByte();
  
  externalizeExecutionState();
  Oop newClosure = closureCopy(numArgs, instructionPointer() + 2 - (method_obj()->as_u_char_p() + Object::BaseHeaderSize), numCopied);
  // Recover from GC, but no Object* 's

  internalizeExecutionState();
  Object_p newClosure_obj = newClosure.as_object();
  newClosure_obj->storePointerUnchecked(Object_Indices::ClosureOuterContextIndex, activeContext());
  reclaimableContextCount = 0; // The closure refers to thisContext so it cannot be reclaimed
  if (numCopied > 0) {
    for (u_int32 i = 0;  i < numCopied;  ++i )
      newClosure_obj->storePointerUnchecked(i + Object_Indices::ClosureFirstCopiedValueIndex, internalStackValue( numCopied - i - 1));
    internalPop(numCopied);
  }
  set_localIP(localIP() + blockSize);
  fetchNextBytecode();
  internalPush(newClosure);
}


Oop Squeak_Interpreter::closureCopy(u_int32 numArgs, u_int32 initialIP, u_int32 numCopied) {
  Object_p newClosure_obj;
  {
    Safepoint_Ability sa(true);
    newClosure_obj = splObj_obj(Special_Indices::ClassBlockClosure)->instantiateSmallClass(
                (Object_Indices::ClosureFirstCopiedValueIndex + numCopied) * sizeof(Oop)  +  Object::BaseHeaderSize);
  }
  // Assume young, use unchecked
  newClosure_obj->storePointerUnchecked(Object_Indices::ClosureStartPCIndex, Oop::from_int(initialIP)),
  newClosure_obj->storePointerUnchecked(Object_Indices::ClosureNumArgsIndex, Oop::from_int(numArgs));
  
  omni_set_domain_for_new_object(newClosure_obj);
    
  return newClosure_obj->as_oop();
}


int Squeak_Interpreter::remoteTempLong_literal_index(u_char* bcp) {
  return bcp[1]; // also bcp[2] but my framework doesn't support it
}

int Squeak_Interpreter::pushRemoteTempLongBytecode_literal_index(u_char* bcp) { return remoteTempLong_literal_index(bcp); }
int Squeak_Interpreter::storeRemoteTempLongBytecode_literal_index(u_char* bcp) { return remoteTempLong_literal_index(bcp); }
int Squeak_Interpreter::storeAndPopRemoteTempLongBytecode_literal_index(u_char* bcp) { return remoteTempLong_literal_index(bcp); }
# endif


int Squeak_Interpreter::extendedStoreBytecode_literal_index(u_char* bcp) {
  u_char d = bcp[1];
  u_char vi = d & 63;
  if (((d >> 6) & 3) == 3) return vi;
  return -1;
}

int Squeak_Interpreter::singleExtendedSendBytecode_literal_index(u_char* bcp) {
  u_char d = bcp[1];
  return d & 0x1f;
}

int Squeak_Interpreter::doubleExtendedDoAnythingBytecode_literal_index(u_char* bcp) {
  u_char b2 = bcp[1];
  u_char b3 = bcp[2];
  switch (b2 >> 5) {
      default: return -1;
      case 0: return b3;
      case 1: return b3;
      case 7: return b3;
  }
}

int Squeak_Interpreter::singleExtendedSuperBytecode_literal_index(u_char* bcp) {
  return bcp[1] & 0x1f;
}

int Squeak_Interpreter::secondExtendedSendBytecode_literal_index(u_char* bcp) {
  u_char descriptor = bcp[1];
  return descriptor & 0x3f;
}


int Squeak_Interpreter::sendLiteralSelectorBytecode_literal_index(u_char* bcp) {
  return *bcp & 0xf;
}

