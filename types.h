#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <iostream>


using OptionalString = std::optional<std::string>;
using OptionalStrings = std::optional<std::vector<std::string>>;
using OptionalPair = std::pair <const std::string&, const OptionalString&>;

std::ostream& operator<< (std::ostream& os, const OptionalPair& v);
