/*
 * Hyenae NG
 *   Advanced Network Packet Generator (NextGen)
 *
 * Copyright (C) 2020 Robin Richter
 *
 *   Contact  : hyenae.tool@googlemail.com
 *   Homepage : https://github.com/r-richter/hyenae-ng
 *
 * This file is part of Hyenae NG.
 *
 * Hyenae NG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hyenae NG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hyenae NG.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../../../include/frontend/console/states/generator_selector.h"
#include "../../../../include/frontend/console/states/ethernet_frame_setup.h"
#include "../../../../include/frontend/console/states/ip_v4_frame_setup.h"
#include "../../../../include/frontend/console/states/ip_v6_frame_setup.h"
#include "../../../../include/frontend/console/states/icmp_v4_frame_setup.h"
#include "../../../../include/frontend/console/states/icmp_v6_frame_setup.h"
#include "../../../../include/frontend/console/states/icmp_echo_payload_setup.h"
#include "../../../../include/frontend/console/states/udp_frame_setup.h"
#include "../../../../include/frontend/console/states/tcp_frame_setup.h"
#include "../../../../include/frontend/console/states/text_buffer_setup.h"

namespace hyenae::frontend::console::states
{
    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        console_app_state* parent) :
            generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_ETHERNET_FRAME |
            GFLAG_TEXT_BUFFER;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        generator_setup* parent) :
            generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_TEXT_BUFFER;
        
    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        ethernet_frame_setup* parent) :
            generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_IP_V4_FRAME |
            GFLAG_IP_V6_FRAME |
            GFLAG_TEXT_BUFFER;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        ip_v4_frame_setup* parent) :
            generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_ICMP_V4_FRAME |
            GFLAG_TCP_FRAME |
            GFLAG_UDP_FRAME |
            GFLAG_TEXT_BUFFER;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        ip_v6_frame_setup* parent) :
        generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_ICMP_V4_FRAME |
            GFLAG_ICMP_V6_FRAME |
            GFLAG_TCP_FRAME |
            GFLAG_UDP_FRAME |
            GFLAG_TEXT_BUFFER;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        icmp_v4_frame_setup* parent) :
            generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_ICMP_V4_ECHO_PAYLOAD;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::generator_selector(
        string_t title,
        console_app_state_context* context,
        console_io* console_io,
        icmp_v6_frame_setup* parent) :
        generator_setup(context, console_io, parent)
    {
        _title = title;

        _generator_flags =
            GFLAG_ICMP_V6_ECHO_PAYLOAD;

    } /* generator_selector */

    /*---------------------------------------------------------------------- */

    generator_selector::~generator_selector()
    {
        for (auto item : _menu_items)
        {
            delete item.first;
            delete item.second;
        }

        safe_delete(_menu);
        safe_delete(_none_item);
        safe_delete(_back_item);

    } /* ~generator_selector */

    /*---------------------------------------------------------------------- */

    bool generator_selector::run()
    {
        if (!_init)
        {
            // Initialize when entered in order to prevent a
            // stack-overflow because of generator_states that
            // might also use generator_selectors.

            inizialize();

            _init = true;
        }
        
        update_menu_items();

        console_menu::item* choice = _menu->prompt(_selected_item);

        if (choice != NULL)
        {
            if (choice != _back_item)
            {
                if (choice != _none_item)
                {
                    if (_selected_setup != _menu_items[choice])
                    {
                        _selected_setup = _menu_items[choice];

                        _selected_setup->on_select();
                    }
                    else
                    {
                        _selected_setup->enter();
                    }

                    _selected_item = choice;
                }
                else
                {
                    _selected_item = NULL;
                    _selected_setup = NULL;
                }
            }
            else
            {
                back();
            }
        }
        
        return true;

    } /* run */

    /*---------------------------------------------------------------------- */

    string_t generator_selector::get_generator_name() const
    {
        if (_selected_setup != NULL)
        {
            return _selected_setup->get_generator_name();
        }
        else
        {
            return "None";
        }

    } /* get_generator_name */

    /*---------------------------------------------------------------------- */

    generator_selector::data_generator_t*
        generator_selector::get_generator() const
    {
        if (_selected_setup != NULL)
        {
            return _selected_setup->get_generator();
        }
        else
        {
            return NULL;
        }
        
    } /* get_generator */

    /*---------------------------------------------------------------------- */

    void generator_selector::update_generator()
    {
        if (_selected_setup != NULL)
        {
            _selected_setup->update_generator();
        }

    } /* update_generator */

    /*---------------------------------------------------------------------- */

    void generator_selector::inizialize()
    {
        using namespace model::generators::protocols;

        _menu = new console_menu(get_console(), _title);

        // None
        _none_item = new console_menu::item("None");
        _menu->add_item(_none_item);

        if (_generator_flags & GFLAG_ETHERNET_FRAME)
        {
            // Ethernet-Frame
            add_generator(new ethernet_frame_setup(
                get_context(), get_console(), get_parent()));
        }

        if (_generator_flags & GFLAG_IP_V4_FRAME)
        {
            // IPv4-Frame
            add_generator(new ip_v4_frame_setup(
                get_context(),
                get_console(),
                get_parent(),
                (ethernet_frame_setup*) get_parent()));
        }

        if (_generator_flags & GFLAG_IP_V6_FRAME)
        {
            // IPv6-Frame
            add_generator(new ip_v6_frame_setup(
                get_context(),
                get_console(),
                get_parent(),
                (ethernet_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_ICMP_V4_FRAME)
        {
            // ICMPv4-Frame
            add_generator(new icmp_v4_frame_setup(
                get_context(),
                get_console(),
                get_parent(),
                (ip_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_ICMP_V6_FRAME)
        {
            // ICMPv6-Frame
            add_generator(new icmp_v6_frame_setup(
                get_context(),
                get_console(),
                get_parent(),
                (ip_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_ICMP_V4_ECHO_PAYLOAD)
        {
            // ICMPv4-Echo Payload
            add_generator(new icmp_echo_payload_setup(
                icmp_echo_payload_generator::ICMP_V4_TYPE,
                icmp_echo_payload_generator::ICMP_V4_CODE,
                get_context(),
                get_console(),
                get_parent(),
                (icmp_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_ICMP_V6_ECHO_PAYLOAD)
        {
            // ICMPv6-Echo Payload
            add_generator(new icmp_echo_payload_setup(
                icmp_echo_payload_generator::ICMP_V6_TYPE,
                icmp_echo_payload_generator::ICMP_V6_CODE,
                get_context(),
                get_console(),
                get_parent(),
                (icmp_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_TCP_FRAME)
        {
            // TCP-Frame
            add_generator(new tcp_frame_setup(
                tcp_frame_generator::PROTOCOL,
                get_context(),
                get_console(),
                get_parent(),
                (ip_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_UDP_FRAME)
        {
            // UDP-Frame
            add_generator(new udp_frame_setup(
                udp_frame_generator::PROTOCOL,
                get_context(),
                get_console(),
                get_parent(),
                (ip_frame_setup*)get_parent()));
        }

        if (_generator_flags & GFLAG_TEXT_BUFFER)
        {
            // Text-Buffer
            add_generator(new text_buffer_setup(
                get_context(), get_console(), get_parent()));
        }

        // Back
        _back_item = new console_menu::item("Back");
        _menu->add_item(_back_item);

    } /* inizialize */

    /*---------------------------------------------------------------------- */

    void generator_selector::update_menu_items()
    {
        _none_item->set_selected(false);

        for (auto item : _menu_items)
        {
            item.first->set_selected(false);
            item.first->set_hint("");
        }

        if (_selected_setup != NULL)
        {
            for (auto item : _menu_items)
            {
                if (item.second == _selected_setup)
                {
                    item.first->set_selected(true);
                    item.first->set_hint("...");

                    break;
                }
            }
        }
        else
        {
            _menu->select_all(false);
            _none_item->set_selected(true);
        }

    } /* update_menu_items */

    /*---------------------------------------------------------------------- */

    void generator_selector::add_generator(generator_setup* setup_state)
    {
        console_menu::item* menu_item = NULL;

        menu_item = new console_menu::item(
            setup_state->get_generator_name());
        
        _menu_items[menu_item] = setup_state;
        _menu->add_item(menu_item);

    } /* add_generator */

    /*---------------------------------------------------------------------- */

} /* hyenae::frontend::console::states */
