#include <stdint.h>

// Macro tools
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__ // Helper When compiling Between MSVS and Clang
#endif

#define _CONCAT_IMPL( x, y ) x##y
#define _MACRO_CONCAT( x, y ) _CONCAT_IMPL( x, y )
#define STRINGIFY(x) #x

// Memory size defines
#define KiB 1024				// Kibibyte
#define MiB 1024 * 1024			// Mebibyte
#define GiB 1024 * 1024 * 1024	// Gibibyte

// Shortened primitives
typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef float		f32;
typedef double		f64;
typedef bool		b8;
typedef char		c8;
typedef wchar_t		w16;

// Limits
#define I8_MIN		std::numeric_limits<i8>	::min()
#define I16_MIN		std::numeric_limits<i16>::min()
#define I32_MIN		std::numeric_limits<i32>::min()
#define I64_MIN		std::numeric_limits<i64>::min()
#define I8_MAX		std::numeric_limits<i8>	::max()
#define I16_MAX		std::numeric_limits<i16>::max()
#define I32_MAX		std::numeric_limits<i32>::max()
#define I64_MAX		std::numeric_limits<i64>::max()
#define U8_MAX		std::numeric_limits<u8>	::max()
#define U16_MAX		std::numeric_limits<u16>::max()
#define U32_MAX		std::numeric_limits<u32>::max()
#define U64_MAX		std::numeric_limits<u64>::max()

#define F32_MIN		std::numeric_limits<f32>::min()
#define F64_MIN		std::numeric_limits<f64>::min()
#define F32_MAX		std::numeric_limits<f32>::max()
#define F64_MAX		std::numeric_limits<f64>::max()

#define F32_EPSILON	std::numeric_limits<f32>::epsilon()
#define F64_EPSILON	std::numeric_limits<f64>::epsilon()