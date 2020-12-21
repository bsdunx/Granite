### Tasks

## TODO

# build system
- support building multiple platform backends where possible

# modernization
- where possible remove legacy includes `malloc.h`, etc. (remembering windows doesn't actually conform to C standards)
- remove legacy constant definitions M_PI, etc.
- investigate removal of Granite::Variant
- rework event handling and use std::make_unique

# dependencies
- add alternatives to rapidjson

# features
- [luajit, luavela] + sol
- plugin system
- better networking support (ASIO)
- serialization

# misc
- add conditional runtime support for fossilize when enabled


## FIXME

# misc
- fix fossilize sampler creation
    Fossilize ERROR: Error: pNext in VkSamplerCreateInfo not supported. (pNext->sType chain: [1000156001]
