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
  oop_int_t sly_ensemble_pointer; /* was: extra_preheader_word */
# endif

# if Include_Domain_In_Object_Header
  // typedef enum foreign_access_policy { 
  //  UNSPECIFIED  = 0, /* Has not been set (not sure yet how to proceed here, probably just ignore and handle as local) */
  //  SYNCHRONOUS  = 1, /* Synchronous direct access is allowed from foreign domains */
  //  ASYNCHRONOUS = 2, /* Access has to be transformed into an asynchronous one, but is allowed then. */
  //  NO_ACCESS    = 3  /* No access allowed */
  // } foreign_access_policy_t;
  
  // static const size_t foreign_access_policy_bits = 2;
  static const size_t logic_id_bits = (sizeof(u_oop_int_t) * 8) - (  3 /*read|write*/
                                                                   * 2 /*sync|async*/
                                                                   * 1 /*bit*/  + Tag_Size); 
  
  // STEFAN: TODO: figure out whether we need to have an exception mechnanism directly specified here,
  //               or whether it is ok to have it in the domain object
  
  typedef union domain_header {
    oop_int_t raw_value;
    struct {
      unsigned                int_tag  : Tag_Size; /* Seems to be necessary, according to David's comment at the top,
                                                      should verify that.
                                                      But anyway, if this preheader word is treated as a normal slot,
                                                      when saving such objects, then it is important to treat
                                                      it as a normal SmallInteger. 
                                                      This way, I do not have to be careful, but can just use the int
                                                      to encode all the interesting information.
                                                      STEFAN 2011-07-10 */
      unsigned logic_id : logic_id_bits;
      
      bool foreign_sync_read     : 1;
      bool foreign_sync_write    : 1;
      bool foreign_sync_execute  : 1;
      bool foreign_async_read    : 1;
      bool foreign_async_write   : 1;
      bool foreign_async_execute : 1;
      
    } __attribute__ ((__packed__)) bits;
  } domain_header_t;
  
  domain_header_t domain;
# endif
  
  static oop_int_t* backpointer_address_from_header_address(void* p) {
    return &((Preheader*)p)[-1].backpointer;
  }
  
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
};

static const int backpointer_oop_size = 1;
static const int backpointer_byte_size = sizeof(oop_int_t);

static const int preheader_byte_size = sizeof(Preheader);
static const int preheader_oop_size = sizeof(Preheader) / sizeof(oop_int_t);
