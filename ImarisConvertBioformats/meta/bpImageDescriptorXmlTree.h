/***************************************************************************
 *   Copyright (c) 2021-present Bitplane AG Zuerich                        *
 *                                                                         *
 *   ImarisConvertBioformats is free software; you can redistribute it     *
 *   and/or modify it under the terms of the GNU General Public License    *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this program.  If not, please see                  *
 *   <http://www.gnu.org/licenses/gpl-2.0.html>.                           *
 ***************************************************************************/


#ifndef BPIMAGEDESCRIPTORXMLTREE_H
#define BPIMAGEDESCRIPTORXMLTREE_H


#include "bpImageDescriptor.h"

#include "bpXmlTree.h"


/**
* Define the xml format for image descriptors
*/
class bpImageDescriptorXmlTree
{
public:
  static bpXmlTree::tPtr ToXml(const bpString& aImagePath, const bpImageDescriptor& aImageDescriptor, bool aMakeRelativePaths = false);
  static bpXmlTree::tPtr ToXml(const std::vector<std::pair<bpString, bpImageDescriptor>>& aImageDescriptors, bool aMakeRelativePaths = false);

  static bool FromXml(const bpXmlTree::tPtr& aXml, bpString& aImagePath, bpImageDescriptor& aImageDescriptor);
  static bool FromXml(const bpXmlTree::tPtr& aXml, std::vector<std::pair<bpString, bpImageDescriptor>>& aImageDescriptors);
};

#endif
