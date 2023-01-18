#if __INCLUDE_LEVEL__
#error "Don't include this file"
#endif

// Yes, the .cpp  file is included.
#include "core/uniqueidobjectstorage.cpp"
#include "iremotenode.h"

template class UniqueIdObjectStorage<ApiGear::ObjectLink::IRemoteNode>;
