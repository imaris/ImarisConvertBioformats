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


#include "bpXmlTree.h"
#include "bpUtils.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-conversion"
#include <boost/property_tree/xml_parser.hpp>
#pragma clang diagnostic warning "-Wsign-conversion"
#else
#include <boost/property_tree/xml_parser.hpp>
#endif

#include <sstream>

const bpString bpXmlTree::mAttrId = "<xmlattr>";
const bpString bpXmlTree::mSeparator = ".";

bpXmlTree::bpXmlTree(const bpString& aXmlString, bool aTrimFormattingContent, bool aRejectMalformedXml)
: mRoot(new tTree()),
  mTree(mRoot.get())
{
  std::istringstream vStream(aXmlString);
  try {
    boost::property_tree::read_xml(vStream, *mTree);
  }
  catch (boost::property_tree::xml_parser_error&) {
    if (aRejectMalformedXml) {
      throw std::runtime_error("bpXmlTree::Invalid input string generated xml_parser_error.");
    }
  }
  catch (boost::property_tree::ptree_error& vError) {
    if (aRejectMalformedXml) {
      throw std::runtime_error(bpString("bpXmlTree::Invalid input string: ") + vError.what());
    }
  }
  tTree::iterator vIt = mTree->begin();
  if (vIt != mTree->end()) {
    mTree = &vIt->second;
    mName = vIt->first;
    if (aTrimFormattingContent) {
      TrimFormattingContent(*mTree);
    }
  }
}

bpXmlTree::bpXmlTree(const tTreePtr& aRoot, tTree& aTree, const bpString& aName)
: mRoot(aRoot),
  mTree(&aTree),
  mName(aName)
{
}

bpXmlTree::~bpXmlTree()
{
}

bool bpXmlTree::operator==(const bpXmlTree& aOther) const
{
  return mTree == aOther.mTree;
}

bool bpXmlTree::operator!=(const bpXmlTree& aOther) const
{
  return mTree != aOther.mTree;
}

bpString bpXmlTree::ToXml() const
{
  tTree vTree;
  vTree.add_child(GetName(), *mTree);
  std::ostringstream vStream;
  boost::property_tree::write_xml(vStream, vTree);
  bpString vXml = vStream.str();
  bpSize vPos = vXml.find('\n');
  return vPos != bpString::npos ? vXml.substr(vPos + 1) : vXml;
}

bpXmlTree::tPtr bpXmlTree::GetParent() const
{
  return GetParent(*mRoot);
}

const bpString& bpXmlTree::GetName() const
{
  return mName;
}

const bpString& bpXmlTree::GetValue() const
{
  return mTree->data();
}

void bpXmlTree::SetValue(const bpString& aValue)
{
  mTree->data() = aValue;
}

bpXmlTree::tPtr bpXmlTree::AddTag(const bpString& aTagName)
{
  return GetTree(mTree->add_child(aTagName, tTree()), aTagName);
}

bpXmlTree::tPtr bpXmlTree::AddTag(const bpXmlTree& aTag)
{
  return GetTree(mTree->add_child(aTag.GetName(), *aTag.mTree), aTag.GetName());
}

void bpXmlTree::DeleteTag(const bpXmlTree& aTag)
{
  tTree::iterator vIt = mTree->begin();
  tTree::iterator vEnd = mTree->end();
  for (; vIt != vEnd; ++vIt) {
    if (&vIt->second == aTag.mTree) {
      mTree->erase(vIt);
      return;
    }
  }
}

bpXmlTree::tPtr bpXmlTree::GetTag(const bpString& aTagName) const
{
  tTree::assoc_iterator vIt = mTree->find(aTagName);
  return vIt != mTree->not_found() ? GetTree(vIt->second, vIt->first) : tPtr();
}

std::vector<bpXmlTree::tPtr> bpXmlTree::GetTags(const bpString& aTagName) const
{
  std::vector<tPtr> vTags;
  tTree::iterator vIt = mTree->begin();
  tTree::iterator vEnd = mTree->end();
  for (; vIt != vEnd; ++vIt) {
    if (vIt->first == aTagName) {
      vTags.push_back(GetTree(vIt->second, vIt->first));
    }
  }
  return vTags;
}

std::vector<bpXmlTree::tPtr> bpXmlTree::GetAllTags() const
{
  std::vector<tPtr> vTags;
  tTree::iterator vIt = mTree->begin();
  tTree::iterator vEnd = mTree->end();
  for (; vIt != vEnd; ++vIt) {
    bpString v(vIt->first);
    if (vIt->first != mAttrId) {
      vTags.push_back(GetTree(vIt->second, vIt->first));
    }
  }
  return vTags;
}

bpXmlTree::tPtr bpXmlTree::GetTree(tTree& aTree, const bpString& aName) const
{
  tPtr vTag(new bpXmlTree(mRoot, aTree, aName));
  return vTag;
}

void bpXmlTree::AddAttr(const bpString& aAttrName, const bpString& aAttrValue)
{
  mTree->put(GetAttrPath(aAttrName), aAttrValue);
}

void bpXmlTree::DeleteAttr(const bpString& aAttrName)
{
  tTree* vTree = GetTreeAttrs(*mTree);
  if (vTree) {
    vTree->erase(aAttrName);
    if (vTree->empty()) {
      mTree->erase(mAttrId);
    }
  }
}

bool bpXmlTree::HasAttr(const bpString& aAttrName) const
{
  const tTree* vTree = GetTreeAttrs(*mTree);
  return vTree && vTree->find(aAttrName) != vTree->not_found();
}

bpString bpXmlTree::GetAttr(const bpString& aAttrName) const
{
  return mTree->get(GetAttrPath(aAttrName), bpString());
}

std::vector<bpString> bpXmlTree::GetAllAttrNames() const
{
  const tTree* vTree = GetTreeAttrs(*mTree);

  std::vector<bpString> vAttrs;
  if (vTree) {
    tTree::const_iterator vIt = vTree->begin();
    tTree::const_iterator vEnd = vTree->end();
    for (; vIt != vEnd; ++vIt) {
      vAttrs.push_back(vIt->first);
    }
  }
  return vAttrs;
}

bpString bpXmlTree::GetAttrPath(const bpString& aAttrName)
{
  return mAttrId + mSeparator + aAttrName;
}

bpXmlTree::tTree* bpXmlTree::GetTreeAttrs(tTree& aTree)
{
  tTree::assoc_iterator vIt = aTree.find(mAttrId);
  return vIt != aTree.not_found() ? &vIt->second : 0;
}

bpXmlTree::tPtr bpXmlTree::GetParent(tTree& aAncestor) const
{
  tTree::iterator vIt = aAncestor.begin();
  tTree::iterator vEnd = aAncestor.end();
  for (; vIt != vEnd; ++vIt) {
    if (&vIt->second == mTree) {
      return GetTree(aAncestor, vIt->first);
    }
    tPtr vParent = GetParent(vIt->second);
    if (vParent) {
      return vParent;
    }
  }
  return tPtr();
}

void bpXmlTree::TrimFormattingContent(tTree& aTree)
{
  aTree.data() = bpTrim(aTree.data());
  tTree::iterator vIt = aTree.begin();
  tTree::iterator vEnd = aTree.end();
  for (; vIt != vEnd; ++vIt) {
    if (vIt->first != mAttrId) {
      TrimFormattingContent(vIt->second);
    }
  }
}

#if defined (_MSC_VER)
// the warning happens after this line (!?)
#pragma warning(disable:4503)
#endif
