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

const char * const OstDomain::field_names[] = { "domainForNewObjects", "domainCustomizations" };

OstDomain The_OstDomain;


void OstDomain::initialize(Oop ostDomain) {
  field_accessor.update_indices(ostDomain);
  domain_selectors = The_Squeak_Interpreter()->splObj(Special_Indices::ArrayOstDomainSelectors);
}


