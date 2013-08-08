/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "random_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    random_pdu::sptr
    random_pdu::make(int min_items, int max_items)
    {
      return gnuradio::get_initial_sptr(new random_pdu_impl(min_items, max_items));
    }

    random_pdu_impl::random_pdu_impl(int min_items, int max_items)
      :	block("random_pdu",
		 io_signature::make (0, 0, 0),
		 io_signature::make (0, 0, 0)),
	d_urange(min_items, max_items),
	d_brange(0, 255),
	d_rvar(d_rng, d_urange),
	d_bvar(d_rng, d_brange)
    {
      message_port_register_out(PDU_PORT_ID);
      message_port_register_in(pmt::mp("generate"));
      set_msg_handler(pmt::mp("generate"), boost::bind(&random_pdu_impl::generate_pdu, this, _1));
    }

    bool
    random_pdu_impl::start() 
    {
      output_random();
      return true;
    }

    void
    random_pdu_impl::output_random()
    {
      // pick a random vector length
      int len = d_rvar();

      // fill it with random bytes
      std::vector<unsigned char> vec;
      for (int i=0; i<len; i++)
        vec.push_back((unsigned char) d_bvar());
      
      // send the vector
      pmt::pmt_t vecpmt(pmt::make_blob(&vec[0], len));
      pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, vecpmt));

      message_port_pub(PDU_PORT_ID, pdu);
    }

  } /* namespace blocks */
}/* namespace gr */
