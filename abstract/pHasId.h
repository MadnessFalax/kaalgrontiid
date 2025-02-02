#pragma once
#include <type_traits>

namespace nspHasId {

	template<class id_type = unsigned int>
	class pHasId {
	public:
		virtual std::enable_if_t<std::is_integral_v<id_type> && std::is_unsigned_v<id_type>, id_type> get_id() = 0;
	};

}