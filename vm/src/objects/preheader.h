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


// The stuff that comes before the squeak header word(s):
//  the backpointer, and maybe other things, too.
// Each word must be tagged with the low order two bits encoding the type of object just as the header must be.
// This invariant was included so that the preheader words could be marked as free and contain the size of the object
//  to speed heap scanning (as best as I can recall).
// -- dmu 3/2010

struct Preheader {
public:
  oop_int_t backpointer; // must be first, for instance for free chuck this is set to give the length

# if Extra_Preheader_Word_Experiment
  /* This word is used to do the dispatch of example messages send to an object */
  // STEFAN: was named extra_preheader_word before. Did not refactor everything, just the name here
  //         since the original name does not tell what it is used for, 
  //         but is consistent in the image and all over the VM.
  oop_int_t ensemble_pointer; /* was: extra_preheader_word */
# endif

# if Include_Domain_In_Object_Header
  typedef union domain_header {
    u_oop_int_t value;
    struct {
      unsigned int_tag; /* Seems to be necessary, according to David's comment at the top, should verify that, STEFAN 2011-07-10 */
    } domain;
  } domain_header_t;
  
  domain_header_t domain;
# endif
  
  static oop_int_t* backpointer_address_from_header_address(void* p) {
    return &((Preheader*)p)[-1].backpointer;
  }
  
  oop_int_t* extra_preheader_word_address() {
# if Extra_Preheader_Word_Experiment
      return &ensemble_pointer;
# else
      return NULL;
# endif
  }
  
  /* Does take care of everything but the backpointer */
  void initialize_preheader() {
# if Extra_Preheader_Word_Experiment
    ensemble_pointer = (0 << Tag_Size) | Int_Tag;
# endif
# if Include_Domain_In_Object_Header
    domain.value = (0 << Tag_Size) | Int_Tag;
# endif
  }
};

static const int backpointer_oop_size = 1;
static const int backpointer_byte_size = sizeof(oop_int_t);

static const int preheader_byte_size = sizeof(Preheader);
static const int preheader_oop_size = sizeof(Preheader) / sizeof(oop_int_t);
