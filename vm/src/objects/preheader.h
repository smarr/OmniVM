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

# if Has_Preheader

struct Preheader {
public:
  
# if Enforce_Backpointer || Use_Object_Table
  oop_int_t backpointer; // must be first, for instance for free chuck this is set to give the length
  
  static oop_int_t* backpointer_address_from_header_address(void* p) { 
    return &((Preheader*)p)[-1].backpointer;
  }

# endif
  
# if Extra_Preheader_Word_Experiment
  /* This word is used to do the dispatch of example messages send to an object */
  // STEFAN: was named extra_preheader_word before. Did not refactor everything, just the name here
  //         since the original name does not tell what it is used for, 
  //         but is consistent in the image and all over the VM.
  oop_int_t sly_ensemble_pointer; /* was: extra_preheader_word */
# endif
    
  oop_int_t* extra_preheader_word_address() {
# if Extra_Preheader_Word_Experiment
      return &sly_ensemble_pointer;
# else
      return NULL;
# endif
  }
  
  oop_int_t* domain_header_address() {
# if Include_Domain_In_Object_Header
    return &domain.raw_value;
# else
    return NULL;
# endif
  }
  
  domain_header_t domain_header() {
# if Include_Domain_In_Object_Header
    return domain;
# else
    return (domain_header_t)0; //STEFAN: this does not work..., won't compile
# endif  
  }
  
  /* Does take care of everything but the backpointer */
  void initialize_preheader() {
# if Extra_Preheader_Word_Experiment
    sly_ensemble_pointer = (0 << Tag_Size) | Int_Tag;
# endif
# if Include_Domain_In_Object_Header
    domain.raw_value = (0 << Tag_Size) | Int_Tag;
# endif
  }
  
  void mark_all_preheader_words_free_for_debugging() {
    if (check_assertions) {
      # if Enforce_Backpointer || Use_Object_Table
      backpointer = 0xe0e0e0e0 /* Oop::Illegals::free_extra_preheader_words, not used because of include dependencies */;
      # endif
      
      # if Extra_Preheader_Word_Experiment
      extra_preheader_word = 0xe0e0e0e0 /* Oop::Illegals::free_extra_preheader_words, not used because of include dependencies */;
      # endif
    }
  }
};

# if Enforce_Backpointer || Use_Object_Table
static const int backpointer_oop_size  = 1;
static const int backpointer_byte_size = sizeof(oop_int_t);
# endif

static const int preheader_byte_size = sizeof(Preheader);
static const int preheader_oop_size  = sizeof(Preheader) / sizeof(oop_int_t);

# else

static const int preheader_byte_size = 0;
static const int preheader_oop_size  = 0;


# endif // !Has_Preheader
