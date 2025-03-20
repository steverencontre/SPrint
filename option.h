// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string_view>
#include <iostream>

#include <cups.h>

#include "types.h"

using OVPair = std::pair <std::string, std::string>;
using OVPairs = std::vector <OVPair>;


OVPair parse_eq (const std::string_view& av);
OVPairs ov_split (std::string_view n_eq_v);
std::ostream& operator<< (std::ostream& os, const OVPairs& avps);



class Printer;


class Value
{
public:
	void SetText (const char *p, const char *q);
	void SetInt (int n);
	void SetRange (int lo, int hi);
	void SetEnum (int n, const char *p, const char *q);

	const std::string& Text() const { return m_Text; }

	friend
	std::ostream& operator<< (std::ostream& os, const Value& v);

private:
	enum { INVALID, TEXT, INT, ENUM, RANGE }  m_Type {INVALID};
	std::string		m_Text;
	OptionalString	m_LocalisedText;
	int						m_Int;
	int						m_Int2;
};



std::ostream& operator<< (std::ostream& os, const Value& v);


class Option
{
public:
	Option (Printer& p, const cups_option_t& o)
		: Option {p, o.name, o.value} {}

	Option (Printer& p, const char *name, const char *defval);

	using ValueList = std::vector <class Value>;
	enum MatchType { CNAME, LNAME, NONE };

	MatchType match (const char *name) const
	{
		if (m_Name == name)
			return CNAME;
		if (m_LocalisedName == name)
			return LNAME;
		return NONE;
	}

	const std::string& Name() const { return m_Name; }
	const std::string& Value() const { return m_Value; }
	bool IsJobOption() const { return m_JobOption; }
	const ValueList& Values() const	{ return m_ValuesSupported; }

	friend std::ostream& operator<< (std::ostream& os, const Option& opt);

	void SetValue (const std::string& value);

	void Rename (const char *cname, const char *lname)
	{
		m_Name = std::string {cname};
		m_LocalisedName = std::string {lname};
	}

private:
	std::string		m_Name;
	std::string		m_Value;
	OptionalString	m_LocalisedName;
	OptionalString	m_LocalisedValue;
	ipp_tag_e			m_ValueType {IPP_TAG_CUPS_INVALID};
	ValueList			m_ValuesSupported;
	bool					m_JobOption {false};
};
