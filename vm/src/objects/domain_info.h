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


/* Got removed: was used for an optimized implementation, but on this
   branch the preheader word is just containing a pointer to a domain
   object which will do all magic on the Smalltalk side. */

class Domain_Info {
public:
  // This is just to mark the class abstract, to avoid that anyone uses this
  // class directly at the moment (helping me with refactroing out the
  // optimized implementation)
  virtual ~Domain_Info() = 0;
};



