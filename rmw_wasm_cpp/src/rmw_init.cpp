#include <iostream>  // REMOVE

#include "rmw_wasm_cpp/rmw_init.hpp"
#include "rmw_wasm_cpp/identifier.hpp"

#include "rcutils/strdup.h"
#include "rcutils/types.h"

#include "rmw/init.h"
#include "rmw/impl/cpp/macros.hpp"
#include "rmw/init_options.h"

extern "C"
{

    rmw_ret_t rmw_init_options_init(
        // const char * identifier, // REMOVE
        rmw_init_options_t * init_options, 
        rcutils_allocator_t allocator)
    {
        std::cout << "[WASM] rmw_init_options_init(start)\n"; // REMOVE
        assert(rmw_wasm_cpp::identifier != NULL);
        RMW_CHECK_ARGUMENT_FOR_NULL(init_options, RMW_RET_INVALID_ARGUMENT);
        RCUTILS_CHECK_ALLOCATOR(&allocator, return RMW_RET_INVALID_ARGUMENT);
        if (NULL != init_options->implementation_identifier) {
            RMW_SET_ERROR_MSG("expected zero-initialized init_options");
            return RMW_RET_INVALID_ARGUMENT;
        }
        init_options->instance_id = 0;
        init_options->implementation_identifier = rmw_wasm_cpp::identifier;
        init_options->allocator = allocator;
        init_options->impl = nullptr;
        init_options->enclave = NULL;
        init_options->domain_id = RMW_DEFAULT_DOMAIN_ID;
        init_options->security_options = rmw_get_default_security_options(); // ???
        init_options->localhost_only = RMW_LOCALHOST_ONLY_DEFAULT;
        std::cout << "[WASM] rmw_init_options_init(end)\n"; // REMOVE
        return RMW_RET_OK;
    }

    rmw_ret_t rmw_init_options_copy(
        // const char * identifier, // REMOVE
        const rmw_init_options_t * src, 
        rmw_init_options_t * dst)
    {
        std::cout << "[WASM] rmw_init_options_copy(start)\n"; // REMOVE
        assert(rmw_wasm_cpp::identifier != NULL);
        RMW_CHECK_ARGUMENT_FOR_NULL(src, RMW_RET_INVALID_ARGUMENT);
        RMW_CHECK_ARGUMENT_FOR_NULL(dst, RMW_RET_INVALID_ARGUMENT);
        if (NULL == src->implementation_identifier) {
            RMW_SET_ERROR_MSG("expected initialized dst");
            return RMW_RET_INVALID_ARGUMENT;
        }
        RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
            src,
            src->implementation_identifier,
            rmw_wasm_cpp::identifier,
            return RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
        if (NULL != dst->implementation_identifier) {
            RMW_SET_ERROR_MSG("expected zero-initialized dst");
            return RMW_RET_INVALID_ARGUMENT;
        }
        const rcutils_allocator_t * allocator = &src->allocator;
        RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);

        rmw_init_options_t tmp = *src;
        tmp.enclave = rcutils_strdup(tmp.enclave, *allocator);
        if (NULL != src->enclave && NULL == tmp.enclave) {
            return RMW_RET_BAD_ALLOC;
        }
        tmp.security_options = rmw_get_zero_initialized_security_options(); // ???
        rmw_ret_t ret =
            rmw_security_options_copy(&src->security_options, allocator, &tmp.security_options);
        if (RMW_RET_OK != ret) {
            allocator->deallocate(tmp.enclave, allocator->state);
            return ret;
        }
        *dst = tmp;
        std::cout << "[WASM] rmw_init_options_copy(end)\n"; // REMOVE
        return RMW_RET_OK;
    }

} // extern "C"