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


# include "headers.h"



# if Include_Domain_In_Object_Header

const char* const Domain_Info::field_names[] = {
  "logicId",
  
# define DEFINE_FLAG_NAME(name) #name,
  DO_ALL_POLICY_FLAGS(DEFINE_FLAG_NAME)
# undef  DEFINE_FLAG_NAME
  
  NULL
};

# endif

