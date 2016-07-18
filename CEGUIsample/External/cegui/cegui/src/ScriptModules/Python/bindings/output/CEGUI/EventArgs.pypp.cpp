// This file has been generated by Py++.

#include "boost/python.hpp"
#include "generators/include/python_CEGUI.h"
#include "EventArgs.pypp.hpp"

namespace bp = boost::python;

void register_EventArgs_class(){

    { //::CEGUI::EventArgs
        typedef bp::class_< CEGUI::EventArgs > EventArgs_exposer_t;
        EventArgs_exposer_t EventArgs_exposer = EventArgs_exposer_t( "EventArgs", bp::init< >("*************************************************************************\n\
            Construction\n\
        *************************************************************************\n") );
        bp::scope EventArgs_scope( EventArgs_exposer );
        EventArgs_exposer.def_readwrite( "handled", &CEGUI::EventArgs::handled, "*************************************************************************\n\
            Data members\n\
        *************************************************************************\n\
        ! handlers should increment this if they handled the event.\n" );
    }

}