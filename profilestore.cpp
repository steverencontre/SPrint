// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <fstream>

#include "profilestore.h"



#include <libxdgbasedir/libxdgbasedir.h>

ProfileStore::ProfileStore()
:
  m_FileName {xdg::config::home() + "/SPrint/profiles.yaml"}
{
  m_Node = YAML::LoadFile (m_FileName);    // note, at this point we don't care if file open or YAML read fails

//  YAML::Emitter e {std::cout};
//  e << m_Node;
}
