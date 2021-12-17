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


#ifndef __BP_XML_TREE__
#define __BP_XML_TREE__

#include "ImarisWriter/interface/bpConverterTypes.h"

#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#pragma GCC system_header

#elif defined _MSC_VER
#pragma warning(push, 1)

#endif

#ifndef Q_MOC_RUN
#include <boost/property_tree/ptree.hpp>
#endif

#if defined _MSC_VER
#pragma warning(pop)
#endif


// http://akrzemi1.wordpress.com/2011/07/13/parsing-xml-with-boost/

/*
 * Encapsulates an XML structure and provides modification functionality.
 * The difference to bpXmlTag is that the memory of added tags and attributes is managed.
 * bpXmlTree also supports multiple tags with the same name.
 * The downside is that it only supports string values and attributes.
 *
 */
class bpXmlTree
{
public:
  typedef bpSharedPtr<bpXmlTree> tPtr;

  // aTrimFormattingContent trims newlines and spaces (formatting) from tags content
  bpXmlTree(const bpString& aXmlString, bool aTrimFormattingContent = false, bool aRejectMalformedXml = true);
  ~bpXmlTree();

  // returns true if its the same object, not if it has the same values
  bool operator==(const bpXmlTree& aOther) const;
  bool operator!=(const bpXmlTree& aOther) const;

  bpString ToXml() const;

  tPtr GetParent() const;
  const bpString& GetName() const;

  const bpString& GetValue() const;
  void SetValue(const bpString& aValue);

  tPtr AddTag(const bpString& aTagName);
  tPtr AddTag(const bpXmlTree& aTag);
  void DeleteTag(const bpXmlTree& aTag);
  tPtr GetTag(const bpString& aTagName) const;
  std::vector<tPtr> GetTags(const bpString& aTagName) const;
  std::vector<tPtr> GetAllTags() const;

  void AddAttr(const bpString& aAttrName, const bpString& aAttrValue);
  void DeleteAttr(const bpString& aAttrName);
  bool HasAttr(const bpString& aAttrName) const;
  bpString GetAttr(const bpString& aAttrName) const;
  std::vector<bpString> GetAllAttrNames() const;

private:
  // private and not implemented (hard to define their meaning, copy data or reference?)
  bpXmlTree(const bpXmlTree& aOther);
  bpXmlTree& operator=(const bpXmlTree& aOther);

  typedef boost::property_tree::ptree tTree;
  typedef bpSharedPtr<tTree> tTreePtr;

  static const bpString mAttrId;
  static const bpString mSeparator;

  static bpString GetAttrPath(const bpString& aAttrName);
  static tTree* GetTreeAttrs(tTree& aTree);

  static void TrimFormattingContent(tTree& aTree);

  bpXmlTree(const tTreePtr& aRoot, tTree& aTree, const bpString& aName);
  tPtr GetTree(tTree& aTree, const bpString& aName) const;

  tPtr GetParent(tTree& aAncestor) const;

  const tTreePtr mRoot;
  tTree* mTree;
  bpString mName;
};

#endif // __BP_XML_TREE__
