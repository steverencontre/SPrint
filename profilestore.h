// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PROFILESTORE_H
#define PROFILESTORE_H

#include <string>

#include <yaml-cpp/yaml.h>


/**
 * @todo write docs
 */
class ProfileStore
{
public:
  ProfileStore();

private:
  std::string m_FileName;
  YAML::Node  m_Node;
};

#endif // PROFILESTORE_H
