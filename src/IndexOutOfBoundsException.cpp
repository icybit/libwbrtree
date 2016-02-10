#include "IndexOutOfBoundsException.h"

using namespace RTree::Tools::Exceptions;

IndexOutOfBoundsException::IndexOutOfBoundsException(std::string t_message)
{
	m_message = t_message;
}

std::string IndexOutOfBoundsException::message()
{
	return "IndexOutOfBoundsException: " + m_message;
}
