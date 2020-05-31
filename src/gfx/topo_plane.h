#ifndef topo_plane_include_file
#define topo_plane_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define topo_plane_width 16
#define topo_plane_height 16
#define topo_plane_size 258
#define topo_plane ((gfx_sprite_t*)topo_plane_data)
extern unsigned char topo_plane_data[258];

#ifdef __cplusplus
}
#endif

#endif
