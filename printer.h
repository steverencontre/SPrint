// SPrint  - Steve's Print Program!

// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PRINTER_H
#define PRINTER_H


#include "option.h"

class Printer
{
public:

	Printer (cups_dest_t *printer);

	~Printer()
	{
		cupsFreeDestInfo (m_Info);
		cupsFreeDests (1, m_Dest);
	}

	const char *LocaliseOption (const char *optname)
	{ return  cupsLocalizeDestOption (m_Connection, m_Dest, m_Info, optname); }

	const char *LocaliseValue (const char *optname, const char *valname)
	{ return cupsLocalizeDestValue (m_Connection, m_Dest, m_Info, optname, valname); }

	ipp_attribute_t *Supported (const char *name)
	{ return cupsFindDestSupported (m_Connection, m_Dest, m_Info, name); }

	void SetOption (const char *name, const std::string& value);

	void CheckOption (const char *name);

	void PrintJob (const std::string& filename);

	void OutputOptions (std::ostream& os, const std::string& oname, const std::string& oval) const;

	void ListMedia();

	friend
	std::ostream& operator<< (std::ostream&, const Printer&);

private:
	cups_dest_t				  *m_Dest;
	std::string					m_Name;
	http_t						  *m_Connection;
	cups_dinfo_t				  *m_Info;
	std::vector<Option>		m_Options;
};

std::ostream& operator<< (std::ostream& os, const Printer& p);

std::ostream& operator<< (std::ostream& os, const cups_media_t& m);



#endif // PRINTER_H
