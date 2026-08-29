/*
 * HLH_gui - gui framework
 *
 * Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com
 *
 * To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH.h"
//-------------------------------------

//#defines
#define LAYOUT_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LAYOUT_MIN(a, b) ((a) < (b) ? (a) : (b))
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
static const int layout_axes[HLH_GUI_LAYOUT_LASTVALUE][2] = {{1, 0}, {0, 1}, {1, 0}};
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations

static void layout_calculate_width(HLH_gui_element *e)
{
   int major = layout_axes[e->flags.layout][0];
   int minor = layout_axes[e->flags.layout][1];
   int32_t size[2] = {0, 0};

   e->size[0] = 0;
   e->size[1] = 0;

   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      if(child->flags.ignore)
      {
         continue;
      }

      layout_calculate_width(child);

      size[major] += child->size[major];
      size[minor] = LAYOUT_MAX(child->size[minor], size[minor]);
   }

   size[0] += e->pad[0][0] + e->pad[1][0];
   size[1] += e->pad[0][1] + e->pad[1][1];
   size[minor] += e->child_gap * (e->child_count - 1);
   int32_t space[2] = {size[0], 0};
   e->size[0] = HLH_gui_element_msg_direct(e, HLH_GUI_MSG_GET_WIDTH, 0, &space);
}

static void layout_calculate_height(HLH_gui_element *e)
{
   int major = layout_axes[e->flags.layout][0];
   int minor = layout_axes[e->flags.layout][1];

   if(e->flags.layout == HLH_GUI_LAYOUT_WRAP)
   {
      int32_t size[2] = {0, 0};
      int32_t height_local = 0;
      int32_t width = e->size[0];
      int32_t width_local = 0;

      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         if(child->flags.ignore)
         {
            continue;
         }

         layout_calculate_height(child);

         if(width_local != 0 && width_local + child->size[0] > width)
         {
            size[1] += height_local;
            width_local = 0;
            height_local = 0;
         }

         height_local = LAYOUT_MAX(height_local, child->size[1]);
         width_local += child->size[0];
      }

      if(width_local != 0)
      {
         size[1] += height_local;
      }
      size[0] += e->pad[0][0] + e->pad[1][0];
      size[1] += e->pad[0][1] + e->pad[1][1];
      size[minor] += e->child_gap * (e->child_count - 1);
      int32_t space[2] = {size[1], e->size[0]};
      e->size[1] = HLH_gui_element_msg_direct(e, HLH_GUI_MSG_GET_HEIGHT, 0, &space);
   }
   else
   {
      int32_t size[2] = {0, 0};
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         if(child->flags.ignore)
         {
            continue;
         }

         layout_calculate_height(child);

         size[major] += child->size[major];
         size[minor] = LAYOUT_MAX(child->size[minor], size[minor]);
      }

      size[0] += e->pad[0][0] + e->pad[1][0];
      size[1] += e->pad[0][1] + e->pad[1][1];
      size[minor] += e->child_gap * (e->child_count - 1);
      int32_t space[2] = {size[1], e->size[0]};
      e->size[1] = HLH_gui_element_msg_direct(e, HLH_GUI_MSG_GET_HEIGHT, 0, &space);
   }
}

void layout_calculate_grow_width(HLH_gui_element *e, int32_t const available_[static 2])
{
   int32_t available[2] = {available_[0], available_[1]};

   int major = layout_axes[e->flags.layout][0];
   int minor = layout_axes[e->flags.layout][1];

   int32_t child_pad[2][2] = {0};
   HLH_gui_element_msg_direct(e, HLH_GUI_MSG_GET_CHILD_PAD, 0, &child_pad);

   available[0] -= e->pad[0][0] + e->pad[1][0] + child_pad[0][0] + child_pad[1][0];
   available[1] -= e->pad[0][1] + e->pad[1][1] + child_pad[0][1] + child_pad[1][1];

   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      available[major] -= child->size[major];
   }
   available[major] -= e->child_gap * (e->child_count - 1);

   // Expand against layout direction
   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
      if(fill[minor] && minor == 0)
      {
         child->size[minor] = available[minor];
      }
   }

   // Expand in layouting direction
   int32_t remaining = available[major];
   while(remaining > 0)
   {
      int32_t smallest = INT32_MAX;
      int32_t second_smallest = INT32_MAX;
      int32_t to_add = remaining;
      if(major != 0)
      {
         break;
      }

      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size < smallest)
         {
            second_smallest = smallest;
            smallest = size;
         }
         if(size > smallest)
         {
            second_smallest = LAYOUT_MIN(second_smallest, size);
         }
      }

      if(second_smallest != INT32_MAX)
      {
         to_add = second_smallest - smallest;
      }
      to_add = LAYOUT_MIN(remaining, to_add);

      int num_to_add = 0;
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size == smallest)
         {
            num_to_add += 1;
         }
      }

      if(num_to_add == 0)
      {
         break;
      }

      int cur = 0;
      int rem = to_add % num_to_add;
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size != smallest)
         {
            continue;
         }

         int32_t add = to_add / num_to_add;
         if(cur < rem)
         {
            add += 1;
         }
         child->size[major] += add;
         remaining -= add;
         cur += 1;
      }
   }

   // Children
   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      if(child->flags.ignore)
      {
         continue;
      }

      layout_calculate_grow_width(child, child->size);
   }
}

// TODO: grow_width and grow_height should really be merged into one...
void layout_calculate_grow_height(HLH_gui_element *e, int32_t const available_[static 2])
{
   int32_t available[2] = {available_[0], available_[1]};

   int major = layout_axes[e->flags.layout][0];
   int minor = layout_axes[e->flags.layout][1];

   int32_t child_pad[2][2] = {0};
   HLH_gui_element_msg_direct(e, HLH_GUI_MSG_GET_CHILD_PAD, 0, &child_pad);

   available[0] -= e->pad[0][0] + e->pad[1][0] + child_pad[0][0] + child_pad[1][0];
   available[1] -= e->pad[0][1] + e->pad[1][1] + child_pad[0][1] + child_pad[1][1];

   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      available[major] -= child->size[major];
   }
   available[major] -= e->child_gap * (e->child_count - 1);

   // Expand against layout direction
   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
      if(fill[minor] && minor == 1)
      {
         child->size[minor] = available[minor];
      }
   }

   // Expand in layouting direction
   int32_t remaining = available[major];
   while(remaining > 0)
   {
      int32_t smallest = INT32_MAX;
      int32_t second_smallest = INT32_MAX;
      int32_t to_add = remaining;
      if(major != 1)
      {
         break;
      }

      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size < smallest)
         {
            second_smallest = smallest;
            smallest = size;
         }
         if(size > smallest)
         {
            second_smallest = LAYOUT_MIN(second_smallest, size);
         }
      }

      if(second_smallest != INT32_MAX)
      {
         to_add = second_smallest - smallest;
      }
      to_add = LAYOUT_MIN(remaining, to_add);

      int num_to_add = 0;
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size == smallest)
         {
            num_to_add += 1;
         }
      }

      if(num_to_add == 0)
      {
         break;
      }

      int cur = 0;
      int rem = to_add % num_to_add;
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         int32_t size = child->size[major];
         bool fill[2] = {child->flags.fill_x, child->flags.fill_y};
         if(!fill[major])
         {
            continue;
         }

         if(size != smallest)
         {
            continue;
         }

         int32_t add = to_add / num_to_add;
         if(cur < rem)
         {
            add += 1;
         }
         child->size[major] += add;
         remaining -= add;
         cur += 1;
      }
   }

   // Children
   for(int i = 0; i < e->child_count; i += 1)
   {
      HLH_gui_element *child = e->children[i];
      if(child->flags.ignore)
      {
         continue;
      }

      layout_calculate_grow_height(child, child->size);
   }
}

void layout_calculate_position(HLH_gui_element *e, HLH_gui_rect available)
{
   e->bounds = available;
   e->size_children[0] = 0;
   e->size_children[1] = 0;

   if(e->child_count == 0)
   {
      return;
   }

   if(e->flags.layout == HLH_GUI_LAYOUT_WRAP)
   {
      int major = layout_axes[e->flags.layout][0];
      int minor = layout_axes[e->flags.layout][1];

      int32_t child_pad[2][2] = {0};
      HLH_gui_element_msg(e, HLH_GUI_MSG_GET_CHILD_PAD, 0, &child_pad);

      HLH_gui_rect child_space = e->bounds;
      child_space.min[0] += e->pad[0][0] + child_pad[0][0];
      child_space.min[1] += e->pad[0][1] + child_pad[0][1];
      child_space.max[0] -= e->pad[1][0] + child_pad[1][0];
      child_space.max[1] -= e->pad[1][1] + child_pad[1][1];

      int32_t pos[2] = {child_space.min[0], child_space.min[1]};
      int32_t height_local = 0;
      int32_t width = e->size[0];
      int32_t width_local = 0;

      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         if(child->flags.ignore)
         {
            continue;
         }

         if(width_local != 0 && width_local + child->size[0] > width)
         {
            child_space.min[1] += child->size[major];
            child_space.min[1] += e->child_gap;
            child_space.min[0] = pos[0];
            width_local = 0;
            height_local = 0;
         }

         layout_calculate_position(child, (HLH_gui_rect){{child_space.min[0], child_space.min[1]},
                                   {child_space.min[0] + child->size[0], child_space.min[1] + child->size[1]}});

         child_space.min[0] += child->size[minor];
         child_space.min[0] += e->child_gap;

         height_local = LAYOUT_MAX(height_local, child->size[1]);
         width_local += child->size[0];

         e->size_children[major] += child->size[major] + e->child_gap;
         e->size_children[minor] += LAYOUT_MAX(e->size_children[minor], child->size[minor]);
      }
   }
   else if(e->flags.layout == HLH_GUI_LAYOUT_VERTICAL)
   {
      int major = layout_axes[e->flags.layout][0];
      int minor = layout_axes[e->flags.layout][1];

      int32_t child_pad[2][2] = {0};
      HLH_gui_element_msg(e, HLH_GUI_MSG_GET_CHILD_PAD, 0, &child_pad);

      HLH_gui_rect child_space = e->bounds;
      child_space.min[0] += e->pad[0][0] + child_pad[0][0];
      child_space.min[1] += e->pad[0][1] + child_pad[0][1];
      child_space.max[0] -= e->pad[1][0] + child_pad[1][0];
      child_space.max[1] -= e->pad[1][1] + child_pad[1][1];
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         if(child->flags.ignore)
         {
            continue;
         }

         int32_t origin[2] = {child_space.min[0], child_space.min[1]};
         if(child->flags.center_x)
         {
            origin[0] += (child_space.max[0] - child_space.min[0] - child->size[0]) / 2;
         }

         layout_calculate_position(child, (HLH_gui_rect){{origin[0], origin[1]},
            {origin[0] + child->size[0], origin[1] + child->size[1]}});
         child_space.min[1] += child->size[1];
         child_space.min[1] += e->child_gap;

         e->size_children[1] += child->size[1] + e->child_gap;
         e->size_children[0] = LAYOUT_MAX(e->size_children[0], child->size[0]);
      }
   }
   else if(e->flags.layout == HLH_GUI_LAYOUT_HORIZONTAL)
   {
      int major = layout_axes[e->flags.layout][0];
      int minor = layout_axes[e->flags.layout][1];

      int32_t child_pad[2][2] = {0};
      HLH_gui_element_msg(e, HLH_GUI_MSG_GET_CHILD_PAD, 0, &child_pad);

      HLH_gui_rect child_space = e->bounds;
      child_space.min[0] += e->pad[0][0] + child_pad[0][0];
      child_space.min[1] += e->pad[0][1] + child_pad[0][1];
      child_space.max[0] -= e->pad[1][0] + child_pad[1][0];
      child_space.max[1] -= e->pad[1][1] + child_pad[1][1];
      for(int i = 0; i < e->child_count; i += 1)
      {
         HLH_gui_element *child = e->children[i];
         if(child->flags.ignore)
         {
            continue;
         }

         layout_calculate_position(child, (HLH_gui_rect){{child_space.min[0], child_space.min[1]},
                                   {child_space.min[0] + child->size[0], child_space.min[1] + child->size[1]}});
         child_space.min[major] += child->size[major];
         child_space.min[major] += e->child_gap;

         e->size_children[major] += child->size[major] + e->child_gap;
         e->size_children[minor] = LAYOUT_MAX(e->size_children[minor], child->size[minor]);
      }
   }
}

void HLH_gui_element_layout(HLH_gui_element *e, HLH_gui_rect space)
{
   if(HLH_gui_element_ignored(e))
   {
      return;
   }


   layout_calculate_width(e);
   if(!e->flags.overlay)
   {
      int32_t space_size[2] = {space.max[0] - space.min[0],
         space.max[1] - space.min[1]};
      layout_calculate_grow_width(e, space_size);
   }
   else
   {
      int32_t space_size[2] = {e->size[0],
         space.max[1] - space.min[1]};
      layout_calculate_grow_width(e, space_size);
   }

   layout_calculate_height(e);
   int32_t space_size[2] = {space.max[0] - space.min[0],
      space.max[1] - space.min[1]};
   layout_calculate_grow_height(e, space_size);

   HLH_gui_rect available = {{space.min[0], space.min[1]},
      {space.min[0] + e->size[0], space.min[1] + e->size[1]}};
   layout_calculate_position(e, available);
}

#undef LAYOUT_MAX
#undef LAYOUT_MIN
//-------------------------------------

