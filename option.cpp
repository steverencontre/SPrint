// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "option.h"

#include "printer.h"



std::ostream& operator<< (std::ostream& os, const Value& v)
{
	switch (v.m_Type)
	{
	case Value::INVALID:
		os << "???";
		break;

	case Value::TEXT:
	case Value::ENUM:
		os << OptionalPair {v.m_Text, v.m_LocalisedText};
		break;

	case Value::INT:
		os << v.m_Int;
		break;

	case Value::RANGE:
		os << v.m_Int << ".." << v.m_Int2;
		break;
	}

	return os;
}



OVPair parse_eq (const std::string_view& av)
{
	auto eq = av.find ('=');
	std::string a, v;

	if (eq != a.npos)
	{
		a = av.substr (0, eq);
		v = av.substr (eq + 1);
	}
	else
	{
		a = av;
		v = "?";
	}
	return {a, v};
}

OVPairs ov_split (std::string_view n_eq_v)
{
	OVPairs avps;

	size_t eq = 0;
	do
	{
		eq = n_eq_v.find (',');
		avps.emplace_back (parse_eq (n_eq_v.substr (0, eq)));
		n_eq_v = n_eq_v.substr (eq + 1);
	}
	while (eq != n_eq_v.npos);

	return avps;
}

std::ostream& operator<< (std::ostream& os, const OVPairs& avps)
{
	os << '[';
	for (size_t i = 0; i < avps.size(); ++i)
	{
		os << avps[i].first << '=' << avps[i].second;
		if (i < avps.size() - 1)
			os << ',';
	}
	os << ']';

	return os;
}



void Value::SetText (const char *p, const char *q)
{
	m_Type = TEXT;
	m_Text.assign (p);
	if (strcmp (p, q))
		m_LocalisedText.emplace (q);
}

void Value::SetInt (int n)
{
	m_Type = INT;
	m_Int = n;
}

void Value::SetRange (int lo, int hi)
{
	m_Type = RANGE;
	m_Int = lo;
	m_Int2 = hi;
}

void Value::SetEnum (int n, const char *p, const char *q)
{
	m_Type = ENUM;
	m_Int = n;
	m_Text.assign (p);
	if (strcmp (p, q))
		m_LocalisedText.emplace (q);
}



Option::Option (Printer& printer, const char *name, const char *defval)
{
	m_Name.assign (name);
	m_Value.assign (defval);

	const char *p = printer.LocaliseOption (name);
	if (strcmp (p, name))
		m_LocalisedName.emplace (p);
//	else
//		std::cerr << "**No localised name for attr " << name << std::endl;

	p = printer.LocaliseValue (name, defval);
	if (strcmp (p, defval))
		m_LocalisedValue.emplace (p);

	auto attr {printer.Supported (name)};
	m_ValueType = ippGetValueTag (attr);

	auto count {ippGetCount (attr) };
	m_ValuesSupported.resize (count);

	const char *text = nullptr;

	for (size_t i = 0; i < count; ++i)
	{
		auto& value {m_ValuesSupported [i]};
		int n = ippGetInteger (attr, i);

		switch (m_ValueType)
		{
		case IPP_TAG_ENUM:
			text = ippEnumString (ippGetName (attr), n);
			value.SetEnum (n, text, printer.LocaliseValue (name, text));
			break;

		case IPP_TAG_NAME:
		case IPP_TAG_STRING:
		case IPP_TAG_KEYWORD:
			text = ippGetString (attr, i, nullptr);
			value.SetText (text, printer.LocaliseValue (name, text));
			break;

		//    case IPP_TAG_DATE:				/* Date/time value */
		//    case IPP_TAG_RESOLUTION:			/* Resolution value */

		case IPP_TAG_INTEGER:
			value.SetInt (n);
			break;

		case IPP_TAG_RANGE:
		{
			int n2;
			n = ippGetRange (attr, i, &n2);
			value.SetRange (n, n2);
			break;
		}
		default:
			std::cerr << "Unhandled value type " << m_ValueType << std::endl;
		}
	}
}


void Option::SetValue (const std::string& value)
{
	if (!m_ValuesSupported.empty())
	{
		auto iter = std::find_if (m_ValuesSupported.begin(), m_ValuesSupported.end(),
			[value] (const class Value& v) { return v.Text() == value;}
		);
		if (iter == m_ValuesSupported.end())
		{
			std::cerr << "Value '" << value << "' not supported by attr '" << m_Name << "'" << std::endl;
			throw std::runtime_error ("invalid value");
		}
	}
	m_Value= value;
	m_JobOption = true;
}


std::ostream& operator<< (std::ostream& os, const Option& opt)
{
	os << OptionalPair {opt.m_Name, opt.m_LocalisedName} << " = " << OptionalPair {opt.m_Value, opt.m_LocalisedValue};
	return os;
}
