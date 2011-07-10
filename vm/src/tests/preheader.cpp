/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    Stefan Marr, Vrije Universiteit Brussel - Initial Implementation
 ******************************************************************************/


# include <gtest/gtest.h>

# include "headers.h"

/**
 * It is supposed to be filled with NULL on initialization.
 */
TEST(Preheader, DomainHeaderSize) {
  // Basic assumption first
  ASSERT_EQ(sizeof(oop_int_t), sizeof(u_oop_int_t));
  
  // Domain header needs to fit into this header word
  ASSERT_EQ(sizeof(u_oop_int_t), sizeof(Preheader::domain_header_t));
  
}