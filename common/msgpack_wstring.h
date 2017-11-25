
#pragma once
#include <string>
#include <msgpack.hpp>

namespace msgpack {
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
		namespace adaptor {

			// Place class template specialization here
			template<>
			struct convert<std::wstring> {
				msgpack::object const& operator()(msgpack::object const& o, std::wstring& v) const {
					if (o.type != msgpack::type::STR) throw msgpack::type_error();
					v = std::wstring((const wchar_t*)o.via.str.ptr, o.via.str.size / 2);
					return o;
				}
			};

			template<>
			struct pack<std::wstring> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, std::wstring const& v) const {
					// packing member variables as an array.
					o.pack_str(v.length() * sizeof(wchar_t));
					o.pack_str_body((const char*)v.c_str(), v.length() * 2);
					return o;
				}
			};

			template <>
			struct object_with_zone<std::wstring> {
				void operator()(msgpack::object::with_zone& o, std::wstring const& v) const {
					o.type = type::STR;
					o.via.str.ptr = (char*)o.zone.allocate_align(v.length() * sizeof(wchar_t));
					o.via.str.size = v.length() * sizeof(wchar_t);
				}
			};

		} // namespace adaptor
	} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack


