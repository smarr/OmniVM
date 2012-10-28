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


// included INTO THE MIDDLE of Squeak_Interpreter


void enforced_pushReceiverVariableBytecode();
void enforced_pushLiteralVariableBytecode();
void enforced_storeAndPopReceiverVariableBytecode();
void enforced_extendedPushBytecode();
void enforced_extendedStoreBytecode();
void enforced_extendedStoreAndPopBytecode();
void enforced_singleExtendedSendBytecode();
void enforced_doubleExtendedDoAnythingBytecode();
void enforced_secondExtendedSendBytecode();
void enforced_singleExtendedSuperBytecode();

void enforced_bytecodePrimAdd();
void enforced_bytecodePrimSubtract();
void enforced_bytecodePrimLessThan();
void enforced_bytecodePrimGreaterThan();
void enforced_bytecodePrimLessOrEqual();
void enforced_bytecodePrimGreaterOrEqual();
void enforced_bytecodePrimEqual();
void enforced_bytecodePrimMultiply();
void enforced_bytecodePrimDivide();
void enforced_bytecodePrimMod();
void enforced_bytecodePrimNotEqual();
void enforced_bytecodePrimMakePoint();
void enforced_bytecodePrimBitShift();
void enforced_bytecodePrimDiv();
void enforced_bytecodePrimBitAnd();
void enforced_bytecodePrimBitOr();

void enforced_bytecodePrimAt();
void enforced_bytecodePrimAtPut();
void enforced_bytecodePrimSize();
void enforced_bytecodePrimNext();
void enforced_bytecodePrimNextPut();
void enforced_bytecodePrimAtEnd();
void enforced_bytecodePrimClass();
void enforced_bytecodePrimBlockCopy();

void enforced_bytecodePrimValue();
void enforced_bytecodePrimValueWithArg();
void enforced_commonBytecodePrimValue(int, int);
void enforced_bytecodePrimDo();
void enforced_bytecodePrimNew();
void enforced_bytecodePrimNewWithArg();
void enforced_bytecodePrimPointX();
void enforced_bytecodePrimPointY();

void enforced_sendLiteralSelectorBytecode();

void enforced_shortConditionalJump();
void enforced_longJumpIfTrue();
void enforced_longJumpIfFalse();




void unenforced_pushReceiverVariableBytecode();
void unenforced_pushLiteralVariableBytecode();
void unenforced_storeAndPopReceiverVariableBytecode();
void unenforced_extendedPushBytecode();
void unenforced_extendedStoreBytecode();
void unenforced_extendedStoreAndPopBytecode();
void unenforced_singleExtendedSendBytecode();
void unenforced_doubleExtendedDoAnythingBytecode();
void unenforced_secondExtendedSendBytecode();
void unenforced_singleExtendedSuperBytecode();

void unenforced_bytecodePrimAdd();
void unenforced_bytecodePrimSubtract();
void unenforced_bytecodePrimLessThan();
void unenforced_bytecodePrimGreaterThan();
void unenforced_bytecodePrimLessOrEqual();
void unenforced_bytecodePrimGreaterOrEqual();
void unenforced_bytecodePrimEqual();
void unenforced_bytecodePrimMultiply();
void unenforced_bytecodePrimDivide();
void unenforced_bytecodePrimMod();
void unenforced_bytecodePrimNotEqual();
void unenforced_bytecodePrimMakePoint();
void unenforced_bytecodePrimBitShift();
void unenforced_bytecodePrimDiv();
void unenforced_bytecodePrimBitAnd();
void unenforced_bytecodePrimBitOr();
void unenforced_bytecodePrimAt();
void unenforced_bytecodePrimAtPut();
void unenforced_bytecodePrimSize();
void unenforced_bytecodePrimNext();
void unenforced_bytecodePrimNextPut();
void unenforced_bytecodePrimAtEnd();
void unenforced_bytecodePrimClass();
void unenforced_bytecodePrimBlockCopy();

void unenforced_bytecodePrimValue();
void unenforced_bytecodePrimValueWithArg();
void unenforced_commonBytecodePrimValue(int, int);
void unenforced_bytecodePrimDo();
void unenforced_bytecodePrimNew();
void unenforced_bytecodePrimNewWithArg();
void unenforced_bytecodePrimPointX();
void unenforced_bytecodePrimPointY();

void unenforced_sendLiteralSelectorBytecode();

void unenforced_shortConditionalJump();
void unenforced_longJumpIfTrue();
void unenforced_longJumpIfFalse();





void pushTemporaryVariableBytecode();
void pushLiteralConstantBytecode();

void storeAndPopTemporaryVariableBytecode();
void pushReceiverBytecode();
void pushConstantTrueBytecode();
void pushConstantFalseBytecode();
void pushConstantNilBytecode();
void pushConstantMinusOneBytecode();
void pushConstantZeroBytecode();
void pushConstantOneBytecode();
void pushConstantTwoBytecode();
void returnReceiver();
void returnTrue();
void returnFalse();
void returnNil();
void returnTopFromMethod();
void returnTopFromBlock();
void unknownBytecode();


void popStackBytecode();
void duplicateTopBytecode();
void pushActiveContextBytecode();
void experimentalBytecode();
void shortUnconditionalJump();
void longUnconditionalJump();

# if Include_Closure_Support
  void pushNewArrayBytecode();
  void pushRemoteTempLongBytecode();
  void storeRemoteTempLongBytecode();
  void storeAndPopRemoteTempLongBytecode();
  void pushClosureCopyCopiedValuesBytecode();

  void pushRemoteTempInVectorAt(u_char, u_char);
  void storeRemoteTempInVectorAt(u_char, u_char);
  Oop closureCopy(u_int32, u_int32, u_int32);
# endif





void bytecodePrimEquivalent();

int pushReceiverVariableBytecode_literal_index(u_char*)  { return -1; }
int pushTemporaryVariableBytecode_literal_index(u_char*)  { return -1; }
int pushLiteralConstantBytecode_literal_index(u_char*)  { return -1; }
int pushLiteralVariableBytecode_literal_index(u_char*)  { return -1; }
int storeAndPopReceiverVariableBytecode_literal_index(u_char*)  { return -1; }
int storeAndPopTemporaryVariableBytecode_literal_index(u_char*)  { return -1; }
int pushReceiverBytecode_literal_index(u_char*)  { return -1; }
int pushConstantTrueBytecode_literal_index(u_char*)  { return -1; }
int pushConstantFalseBytecode_literal_index(u_char*)  { return -1; }
int pushConstantNilBytecode_literal_index(u_char*)  { return -1; }
int pushConstantMinusOneBytecode_literal_index(u_char*)  { return -1; }
int pushConstantZeroBytecode_literal_index(u_char*)  { return -1; }
int pushConstantOneBytecode_literal_index(u_char*)  { return -1; }
int pushConstantTwoBytecode_literal_index(u_char*)  { return -1; }
int returnReceiver_literal_index(u_char*)  { return -1; }
int returnTrue_literal_index(u_char*)  { return -1; }
int returnFalse_literal_index(u_char*)  { return -1; }
int returnNil_literal_index(u_char*)  { return -1; }
int returnTopFromMethod_literal_index(u_char*)  { return -1; }
int returnTopFromBlock_literal_index(u_char*)  { return -1; }
int unknownBytecode_literal_index(u_char*)  { return -1; }
int extendedPushBytecode_literal_index(u_char*)  { return -1; }

int extendedStoreBytecode_literal_index(u_char*);
int extendedStoreAndPopBytecode_literal_index(u_char*)  { return -1; }
int singleExtendedSendBytecode_literal_index(u_char*);
int doubleExtendedDoAnythingBytecode_literal_index(u_char*);
int singleExtendedSuperBytecode_literal_index(u_char*);
int secondExtendedSendBytecode_literal_index(u_char*);
int popStackBytecode_literal_index(u_char*)  { return -1; }
int duplicateTopBytecode_literal_index(u_char*)  { return -1; }
int pushActiveContextBytecode_literal_index(u_char*)  { return -1; }
int experimentalBytecode_literal_index(u_char*)  { return -1; }
int shortUnconditionalJump_literal_index(u_char*)  { return -1; }
int shortConditionalJump_literal_index(u_char*)  { return -1; }
int longUnconditionalJump_literal_index(u_char*)  { return -1; }
int longJumpIfTrue_literal_index(u_char*)  { return -1; }
int longJumpIfFalse_literal_index(u_char*)  { return -1; }


int bytecodePrimAdd_literal_index(u_char*)  { return -1; }
int bytecodePrimSubtract_literal_index(u_char*)  { return -1; }
int bytecodePrimLessThan_literal_index(u_char*)  { return -1; }
int bytecodePrimGreaterThan_literal_index(u_char*)  { return -1; }
int bytecodePrimLessOrEqual_literal_index(u_char*)  { return -1; }
int bytecodePrimGreaterOrEqual_literal_index(u_char*)  { return -1; }
int bytecodePrimEqual_literal_index(u_char*)  { return -1; }
int bytecodePrimNotEqual_literal_index(u_char*)  { return -1; }
int bytecodePrimMultiply_literal_index(u_char*)  { return -1; }
int bytecodePrimDivide_literal_index(u_char*)  { return -1; }
int bytecodePrimMod_literal_index(u_char*)  { return -1; }
int bytecodePrimMakePoint_literal_index(u_char*)  { return -1; }
int bytecodePrimBitShift_literal_index(u_char*)  { return -1; }
int bytecodePrimDiv_literal_index(u_char*)  { return -1; }
int bytecodePrimBitAnd_literal_index(u_char*)  { return -1; }
int bytecodePrimBitOr_literal_index(u_char*)  { return -1; }

int bytecodePrimAt_literal_index(u_char*)  { return -1; }
int bytecodePrimAtPut_literal_index(u_char*)  { return -1; }
int bytecodePrimSize_literal_index(u_char*)  { return -1; }
int bytecodePrimNext_literal_index(u_char*)  { return -1; }
int bytecodePrimNextPut_literal_index(u_char*)  { return -1; }
int bytecodePrimAtEnd_literal_index(u_char*)  { return -1; }
int bytecodePrimEquivalent_literal_index(u_char*)  { return -1; }
int bytecodePrimClass_literal_index(u_char*)  { return -1; }
int bytecodePrimBlockCopy_literal_index(u_char*)  { return -1; }
int bytecodePrimValue_literal_index(u_char*)  { return -1; }
int bytecodePrimValueWithArg_literal_index(u_char*)  { return -1; }
int bytecodePrimDo_literal_index(u_char*)  { return -1; }
int bytecodePrimNew_literal_index(u_char*)  { return -1; }
int bytecodePrimNewWithArg_literal_index(u_char*)  { return -1; }
int bytecodePrimPointX_literal_index(u_char*)  { return -1; }
int bytecodePrimPointY_literal_index(u_char*)  { return -1; }

int sendLiteralSelectorBytecode_literal_index(u_char*);

# if Include_Closure_Support
int pushNewArrayBytecode_literal_index(u_char*)  { return -1; }
int pushRemoteTempLongBytecode_literal_index(u_char*);
int storeRemoteTempLongBytecode_literal_index(u_char*);
int storeAndPopRemoteTempLongBytecode_literal_index(u_char*);

int remoteTempLong_literal_index(u_char*);
int pushClosureCopyCopiedValuesBytecode_literal_index(u_char*)  { return -1; }

# endif



