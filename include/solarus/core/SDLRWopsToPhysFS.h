#ifndef SDLRWOPSTOPHYSFS_H
#define SDLRWOPSTOPHYSFS_H

#include <SDL_rwops.h>
#include <physfs.h>
#include <memory>

namespace Solarus {

inline void SDL_RWopsDeleter(SDL_RWops* ops) {
    SDL_RWclose(ops);
    SDL_FreeRW(ops);
}

using SharedRW = std::shared_ptr<SDL_RWops>;

struct PhysfsIOWrapper{
  SharedRW ops;
  PHYSFS_sint64 position;
};

inline PHYSFS_sint64 read(PHYSFS_Io* io, void *buffer, PHYSFS_uint64 len) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  long r = SDL_RWread(wrap->ops.get(),buffer,1,len);
  wrap->position += r;
}

inline PHYSFS_sint64 write(struct PHYSFS_Io *io, const void *buffer, PHYSFS_uint64 len) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  long r = SDL_RWwrite(wrap->ops.get(),buffer,1,len);
  wrap->position += r;
  return r;
}

inline int seek(struct PHYSFS_Io *io, PHYSFS_uint64 offset) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  int pos = SDL_RWseek(wrap->ops.get(),offset,RW_SEEK_SET);
  if(pos == offset)
    wrap->position = pos;
  return pos == offset;
}

inline PHYSFS_sint64 tell(PHYSFS_Io* io) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  return wrap->position;
}

inline PHYSFS_sint64 length(PHYSFS_Io* io) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  return SDL_RWsize(wrap->ops.get());
}

inline PHYSFS_Io* duplicate(PHYSFS_Io* io) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  PHYSFS_Io* nio = new PHYSFS_Io(*io);
  PhysfsIOWrapper* iow = new PhysfsIOWrapper(*wrap);
  nio->opaque = iow;
  return nio;
}

inline int flush(PHYSFS_Io* io) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  return 0;
}

inline void destroy(PHYSFS_Io* io) {
  PhysfsIOWrapper* wrap = reinterpret_cast<PhysfsIOWrapper*>(io->opaque);
  delete wrap;
  delete io;
}

inline PHYSFS_Io* make_io_from_sdl(SDL_RWops* ops) {
  PHYSFS_Io* io = new PHYSFS_Io();
  PhysfsIOWrapper* iow = new PhysfsIOWrapper();
  io->opaque = iow;
  iow->ops = SharedRW(ops,SDL_RWopsDeleter);
  io->read = &read;
  io->write = write;
  io->seek = seek;
  io->tell = tell;
  io->length = length;
  io->duplicate = duplicate;
  io->flush = flush;
  io->destroy = destroy;
  return io;
}

}
#endif // SDLRWOPSTOPHYSFS_H
