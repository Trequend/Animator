#pragma once

#define CEREAL_SERIALIZE_FUNCTION_NAME Serialize
#define CEREAL_LOAD_FUNCTION_NAME Load
#define CEREAL_SAVE_FUNCTION_NAME Save
#define CEREAL_LOAD_MINIMAL_FUNCTION_NAME LoadMinimal
#define CEREAL_SAVE_MINIMAL_FUNCTION_NAME SaveMinimal
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>
