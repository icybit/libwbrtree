#include "IllegalArgumentException.h"

using namespace RTree::Tools::Exceptions;

IllegalArgumentException::IllegalArgumentException(std::string t_message)
{
	m_message = t_message;
}

std::string RTree::Tools::Exceptions::IllegalArgumentException::message()
{
	return "IllegalArgumentException: " + m_message;
}
