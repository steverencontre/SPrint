// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 steve <steve@rsn-tech.co.uk>
// SPDX-License-Identifier: GPL-3.0-or-later


#include "printer.h"
#include "profilestore.h"


#include "/home/steve/OpenSource/C++ arg parsers/argparse/include/argparse/argparse.hpp"


struct Args : public argparse::Args
{
	OptionalString&		attrs		= kwarg ("a,attrs",		"specify attributes; use ? to list attrs, attr=? or ?=? for everything", "attr=value[,attr=value]...");
	OptionalString&		info		= kwarg ("i,info",		"show profile details", "<profile>");
	OptionalString&		load		= kwarg ("l,load",		"load existing profile [use ? for list of available choices]", "<profile>");
	OptionalString&		printer	= kwarg ("p,printer",	"select printer by name [head match, use ? for a list of available choices]", "<printer>");
	OptionalString&		media	= kwarg ("m,media",	"specify media type; use ? to list supported types", "<type>");
	OptionalString&		save		= kwarg ("s,save",		"save resultant profile to specified name", "<profile>");
	OptionalString&		msize	= kwarg ("z,size",		"specify media size; use ? to list supported sizes", "<size>");
	OptionalStrings&	files		= arg ("<image>...",	"one or more image files to print");
};


std::ostream& operator<< (std::ostream& os, const OptionalPair& op)
{
	if (op.second.has_value())
		os << op.second.value() << " [" << op.first << "]";
	else
		os << op.first;

	return os;
}




int main (int argc, char *argv[])
{
	auto profilestore {std::make_unique<ProfileStore>()};

	auto args {argparse::parse<Args> (argc, argv)};

	void *enumdata = &args.printer;

	cupsEnumDests (CUPS_DEST_FLAGS_NONE, 5000, nullptr, 0, 0,
		[] (void *p, unsigned flags, cups_dest_t *dest) -> bool
		{
			const auto& name = ** (OptionalString **) p;

			if (name.has_value())
			{
				if (std::string {dest->name}.contains (name.value()))
				{
					cupsCopyDest (dest, 0, (cups_dest_t **) p);
					return false;
				}
			}
			else
				std::cout << dest->name << std::endl;

			if (flags & CUPS_DEST_FLAGS_MORE)
				return true; // continue enumeration (zero would stop)

			* (void **) p = nullptr;
			return true;
		},
		&enumdata);

	cups_dest_t *dest = (cups_dest_t *) enumdata;

	if (!dest)
	{
		if (args.printer.has_value())
			std::cerr << "Printer '" << args.printer.value() << "' not found" << std::endl;

		return 0;
	}

	Printer printer {dest};

	OVPairs ov_pairs;
	bool query {false};
	if (args.attrs.has_value())
	{
		std::string_view a {args.attrs.value() };
		ov_pairs = ov_split (a);

		for (const auto& ov : ov_pairs)
		{
			if (ov.first == "?" || ov.second == "?")
			{
				printer.OutputOptions (std::cout, ov.first, ov.second);
				query = true;
			}
			else
				printer.SetOption (ov.first.c_str(), ov.second);
		}
	}

	if (args.media.has_value())
	{
		if (args.media == "?")
			printer.ListMedia();
		else
			printer.SetOption (CUPS_MEDIA_TYPE, args.media.value());
	}

	if (!query && args.files.has_value())
	{
		for (const auto& file : args.files.value())
			printer.PrintJob (file);
	}

	return 0;
}
