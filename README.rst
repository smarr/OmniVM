OmniVM - The RoarVM with support for the Ownership-based Metaobject Protocol
============================================================================

The Ownership-based Metaobject Protocol (OMOP) is a unifying substrate for the
implementation of concurrent programming abstractions. Similar to other
metaobject protocols, it allows developers to change the language's behavior
to provide different concurrent programming models. The OmniVM is a
SmalltalkVM and a fork of the RoarVM_.

An overview of the research related to the OMOP can be found at:
http://www.stefan-marr.de/research/omni/


.. _RoarVM:                   http://github.com/smarr/RoarVM


Install and Use
---------------

Please see the INSTALL.md file for installation instructions. In addition to
the OmniVM, you will need the corresponding Smalltalk code, which is listed
there as well.

Features
--------

The OmniVM is compatible with the RoarVM, the Squeak interpreter, and the
CogVM. It provides merely additional support for the ownership-based
metaobject protocol that facilitates the implementation of concurrent
programming abstractions.

Technical Overview
------------------

The implementation details of the OmniVM are currently documented in:

  [1] Supporting Concurrency Abstractions in High-level Language Virtual
      Machines.
      Stefan Marr, PhD Dissertation. Software Languages Lab, Vrije
      Universiteit Brussel, Belgium, VUB Press, January 2013.
      ISBN: 978-90-5718-256-3
      http://www.stefan-marr.de/downloads/marr-phd-2013-supporting-concurrency-abstractions-in-high-level-language-virtual-machines.pdf
  
  [2] Identifying A Unifying Mechanism for the Implementation of Concurrency
      Abstractions on Multi-Language Virtual Machines.
      Stefan Marr and Theo D'Hondt, 50th International Conference on Objects,
      Models, Components, Patterns. TOOLS 2012, Volume 7304 of Lecture Notes
      in Computer Science, page 171-186. Springer, May 2012.
      http://www.stefan-marr.de/downloads/tools12-smarr-dhondt-identifying-a-unifying-mechanism-for-the-implementation-of-concurrency-abstractions-on-multi-language-virtual-machines.pdf

The OmniVM is a simple bytecode interpreter that provides supports an
ownership-based metaobject protocol. Currently there are two implementations
in this repository. The first one checks in every bytecode whether it's
operation needs to be reified by invoking an intercession handler of the
metaobject protocol, and the second implementation uses an extended bytecode
set, where the bytecodes for the enforced mode always reify the operations.
Please see the `omop/...` branches and the PhD dissertation for details.


License
-------

Copyright (c) 2012 - 2013 Stefan Marr and others.
All rights reserved. The OmniVM and the accompanying materials are made
available under the terms of the Eclipse Public License v1.0 which accompanies
this distribution, and is available at:

  http://www.eclipse.org/legal/epl-v10.html

All parts directly taken over from the RoarVM or the original Squeak source
code are licensed under their original licenses.

Credits
-------

The OmniVM is based on the RoarVM_ and we would like to acknowledge the
work of the RoarVM developer for providing this platform.
