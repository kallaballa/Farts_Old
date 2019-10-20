local lub = require 'lub'
local dub = require 'dub'

local inspector = dub.Inspector {
  INPUT    = {
    lub.path '|src/Tonic',
  },
}
local binder = dub.LuaBinder()

binder:bind(inspector, {
  -- Mandatory library name. This is used as prefix for class types.
  lib_name = 'tonic',

  output_directory = lub.path '|src/bind',

  -- Remove this part in included headers
  header_base = lub.path '|include',
  single_lib = 'tonic'

})
