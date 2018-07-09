//Copyright 2018 John Jekel
//See https://github.com/JZJisawesome/15Slide/blob/master/LICENSE for the terms
#ifndef SLIDEFILEDIALOG_H
#define SLIDEFILEDIALOG_H

#include <gtkmm.h>

#include <string>

namespace GTKSlide
{
class SlideFileDialog : public Gtk::FileChooserDialog
{
public:
    SlideFileDialog(Gtk::Window &parent, const std::string &title, Gtk::FileChooserAction action);
    virtual ~SlideFileDialog();

    void display();
};
}

#endif // SLIDEFILEDIALOG_H
