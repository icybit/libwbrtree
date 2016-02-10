#include "IllegalStateException.h"

using namespace RTree::Tools::Exceptions;

IllegalStateException::IllegalStateException(std::string t_message)
{
	m_message = t_message;
}

std::string IllegalStateException::message()
{
	return "IllegalStateException: " + m_message;
}
