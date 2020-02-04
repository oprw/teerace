//mkRace
#include <sstream>
#include <ctime>
#include <vector>

#ifndef MKJSON
#define MKJSON

#include <engine/server.h>

inline std::string escape_json(const std::string &s) {
	std::ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); c++) {
		switch (*c) {
			case '\x00': o << "\\u0000"; break;
			case '\x01': o << "\\u0001"; break;
			case '\x0a': o << "\\n"; break;
			case '\x1f': o << "\\u001f"; break;
			case '\x22': o << "\\\""; break;
			case '\x5c': o << "\\\\"; break;
			default: o << *c;
		}
	}
	return "\""+o.str()+"\"";
}
inline std::string escape_json(int i) {
	std::stringstream s;
	s << i;
	return s.str();
}
inline std::string escape_json(std::vector<std::string> vec) {
	std::string s="{";
	for (long unsigned int i=0; i<vec.size(); i+=2){
		s+=escape_json(vec[i]) + " : " + escape_json(vec[i+1]);
		if(i < vec.size()-2) s+=", ";
	}
	return s+"}";
}

inline std::string encode_json_impl(int pos) { return ""; };
template<typename Arg1, typename Arg2, typename... RestArgs>
std::string encode_json_impl(int pos, Arg1 a1, Arg2 a2, RestArgs... restargs){
	std::string s;
	if(pos) s+=", ";

	s += escape_json(a1) + " : " + escape_json(a2);
	s += encode_json_impl(pos +1, restargs...);
	return s;
}

template<typename... Args>
std::string mkjson( Args... args){
	return "{"+encode_json_impl(0, args...)+"}";
}

#include <iostream>
template<typename... Args>
inline void dumpjson(Args... args){
	std::stringstream timestamp;
	timestamp << std::time(nullptr);
	std::printf("%s\n", mkjson(args..., "ts", timestamp.str()).c_str());
	fflush(stdout);
}

inline std::vector<std::string> json_plr(IServer* Server,int id) {
	std::stringstream idstr;
        idstr << id;
	std::vector<std::string> plr = {"id", idstr.str(), "name", Server->ClientName(id)};
	return plr;
};

#endif
