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

TEST(Preheader, Initialization) {
  struct Preheader h;
  
  h.initialize_preheader();
  
  ASSERT_EQ(1, Int_Tag);

# if Extra_Preheader_Word_Experiment
  ASSERT_EQ(Int_Tag, h.sly_ensemble_pointer);
# endif
  
  ASSERT_EQ(Int_Tag, h.domain.raw_value);
  ASSERT_TRUE(h.domain.bits.int_tag);
  
  ASSERT_TRUE(Oop::from_bits(h.domain.raw_value).is_int());
}

TEST(Preheader, DomainHeaderLayout) {
  /* Verifying some basic assumptions about the bit layout */
  
  
  struct Preheader h;
  h.initialize_preheader();
  
  h.domain.raw_value = 0xFFFFFFFF;
  
  ASSERT_TRUE(Oop::from_bits(h.domain.raw_value).is_int());
  
  ASSERT_EQ(0xFFFFFFFF, h.domain_header().raw_value);
  ASSERT_EQ(0x01FFFFFF, h.domain_header().bits.logic_id);  // That is the max value for the logic id
  
  ASSERT_EQ(Preheader::NO_ACCESS, h.domain_header().bits.read);    // NO_ACCESS corresponds to all bits set
  ASSERT_EQ(Preheader::NO_ACCESS, h.domain_header().bits.write);   // NO_ACCESS corresponds to all bits set
  ASSERT_EQ(Preheader::NO_ACCESS, h.domain_header().bits.execute); // NO_ACCESS corresponds to all bits set

  // Now clear the bits and verify the resulting raw_value
  h.domain.bits.read    = Preheader::UNSPECIFIED;
  h.domain.bits.write   = Preheader::UNSPECIFIED;
  h.domain.bits.execute = Preheader::UNSPECIFIED;  
  
  ASSERT_EQ(0x3FFFFFF, h.domain_header().raw_value);   // This basically says, that the policy flags are the highest (most significant)
  ASSERT_EQ(0x1FFFFFF, h.domain_header().bits.logic_id);
  
  ASSERT_TRUE(Oop::from_bits(h.domain.raw_value).is_int());
}
