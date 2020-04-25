#pragma once

// Assert wrapper for better output when debugging
#ifndef NDEBUG
	#include <iostream>     // for std::cerr
	#define Assert(Expr, Msg) _Assert(#Expr, Expr, __FILE__, __LINE__, Msg)

	void _Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg)
	{
		if (!expr)
		{
			std::cerr << "Assert failed:\t" << msg << "\n"
				<< "Expected:\t" << expr_str << "\n"
				<< "Source:\t\t" << file << ", line " << line << "\n";
			abort();
		}
	}
#else
#   define Assert(Expr, Msg) ;
#endif
