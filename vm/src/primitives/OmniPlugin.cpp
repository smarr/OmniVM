/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    Stefan Marr, Vrije Universiteit Brussel - Project OMNI
 ******************************************************************************/


/** Primitives for Project Omni **/

# include "headers.h"

static const char *moduleName =
#ifdef SQUEAK_BUILTIN_PLUGIN
"OmniPlugin v0.1 15 July 2011 (i)"
#else
"OmniPlugin v0.1 15 July 2011 (e)"
#endif
;

static const char* getModuleName() {
  return moduleName;
}

static Object_Field_Accessor field_accessor_ODomain = 
                        Object_Field_Accessor(Preheader::Domain_Field_Names,
                                              Preheader::Domain_Field_Count);



/** {{{ OMirror class>>primitiveGenerateDomainInfoFrom: aDomain
              ^ aDomainInfo }}} */
static int primitiveGenerateDomainInfoFrom() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  const int ARG_CNT = 1;
  
  if (interp->get_argumentCount() != ARG_CNT) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domain_oop = interp->stackObjectValue(0);
  
  Preheader::domain_header_t domain;
  domain.bits.int_tag = Int_Tag;  // needs to be correctly initialized
  
  domain.bits.foreignSyncRead     = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignSyncRead)     == interp->roots.trueObj;
  domain.bits.foreignSyncWrite    = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignSyncWrite)    == interp->roots.trueObj;
  domain.bits.foreignSyncExecute  = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignSyncExecute)  == interp->roots.trueObj;
  domain.bits.foreignAsyncRead    = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignAsyncRead)    == interp->roots.trueObj;
  domain.bits.foreignAsyncWrite   = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignAsyncWrite)   == interp->roots.trueObj;
  domain.bits.foreignAsyncExecute = field_accessor_ODomain.get_field(domain_oop, Preheader::foreignAsyncExecute) == interp->roots.trueObj;
  

  
  Oop logicId_oop = field_accessor_ODomain.get_field(domain_oop, Preheader::logicId);
  
  if (logicId_oop.is_int())
    domain.bits.logicId = logicId_oop.integerValue();
  else
    domain.bits.logicId = -1;

  interp->popThenPush(ARG_CNT + 1, Oop::from_bits(domain.raw_value));
  return 0;
}


/** {{{ OMirror class>>primitiveDecodeDomainInfo: aDomainInfo into: aDomain 
              ^ aDomain }}} */
static int primitiveDecodeDomainInfoInto() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  const int ARG_CNT = 2;
  
  if (interp->get_argumentCount() != ARG_CNT) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop target     = interp->stackObjectValue(0);
  Oop domainInfo = interp->stackValue(1);
  
  /* Making sure we got all arguments and domainInfo is a SmallInt */
  if (interp->failed()  ||  !domainInfo.is_int())
    return 0;
  
  Preheader::domain_header_t domain;
  domain.raw_value = domainInfo.bits();
  
  field_accessor_ODomain.set_field(target, Preheader::foreignSyncRead,     domain.bits.foreignSyncRead     ? interp->roots.trueObj : interp->roots.falseObj);
  field_accessor_ODomain.set_field(target, Preheader::foreignSyncWrite,    domain.bits.foreignSyncWrite    ? interp->roots.trueObj : interp->roots.falseObj);
  field_accessor_ODomain.set_field(target, Preheader::foreignSyncExecute,  domain.bits.foreignSyncExecute  ? interp->roots.trueObj : interp->roots.falseObj);
  field_accessor_ODomain.set_field(target, Preheader::foreignAsyncRead,    domain.bits.foreignAsyncRead    ? interp->roots.trueObj : interp->roots.falseObj);
  field_accessor_ODomain.set_field(target, Preheader::foreignAsyncWrite,   domain.bits.foreignAsyncWrite   ? interp->roots.trueObj : interp->roots.falseObj);
  field_accessor_ODomain.set_field(target, Preheader::foreignAsyncExecute, domain.bits.foreignAsyncExecute ? interp->roots.trueObj : interp->roots.falseObj);
  

  field_accessor_ODomain.set_field(target, Preheader::logicId, Oop::from_int(domain.bits.logicId));
  
  if (interp->failed())
    return 0;
  
  interp->popThenPush(ARG_CNT + 1, target);
  return 0;
}


/** {{{ OMirror class>>#primitiveGetDomainInfoFor: anObject 
            ^ aDomainInfo }}} **/
static int primitiveGetDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 1) {
    interp->primitiveFail();
    return 0;
  }
   
  Oop o = interp->stackObjectValue(0);
  if (interp->failed()) {
    return 0;
  }
  
  interp->popThenPush(2, o.as_object()->domain_header_oop());
  
  return 0;
}


/** {{{ OMirror class>>#primitiveSetDomainInfoFor: anObject to: aDomainInfo }}} **/
static int primitiveSetDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 2) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domainInfo = interp->stackValue(0);

  if (not domainInfo.is_int()) {
    interp->primitiveFail();
    return 0;
  }
  
  
  Oop target = interp->stackObjectValue(1);
  if (interp->failed()) {
    return 0;
  }

  target.as_object()->set_domain_header(domainInfo);
  interp->pop(2);
  
  return 0;
}

/** Required to be picked up by the loading mechanism as an internal plugin */
static int setInterpreter(struct VirtualMachine* /* anInterpreter */) {
	return 1;
}


void* OmniPlugin_exports[][3] = {
  {(void*) "OmniPlugin", (void*)"primitiveGetDomainInfo", (void*)primitiveGetDomainInfo},
  {(void*) "OmniPlugin", (void*)"primitiveSetDomainInfo", (void*)primitiveSetDomainInfo},
  
  {(void*) "OmniPlugin", (void*)"primitiveGenerateDomainInfoFrom", (void*)primitiveGenerateDomainInfoFrom},
  {(void*) "OmniPlugin", (void*)"primitiveDecodeDomainInfoInto",   (void*)primitiveDecodeDomainInfoInto},
  
  
  /* Required by the internal loading mechanism */
  {(void*) "OmniPlugin", (void*)"setInterpreter",         (void*)setInterpreter},
  {NULL, NULL, NULL}
};

