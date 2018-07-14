/* Copyright 2018 John Jekel
 * This file is part of 15Slide.
 *
 * 15Slide is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 15Slide is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with 15Slide.  If not, see <https://www.gnu.org/licenses/>
*/
#include "GTKSlide/MainWindow.h"

#include "Grid15/Grid.h"
#include "Grid15/GridHelp.h"
#include "GTKSlide/AboutSlide.h"
#include "GTKSlide/SlideFileDialog.h"
#include "GTKSlide/SaveManager.h"
#include "CommandUI.h"

#include "ProgramStuff.h"

#include <gtkmm.h>
#include <iostream>

#include <memory>
#include <exception>

namespace GTKSlide
{
MainWindow::MainWindow(Glib::RefPtr<Gtk::Application> &application, std::shared_ptr<Grid15::Grid> &newGridPtr)
    : saveManager{new SaveManager {}}, tileGrid{*this, newGridPtr, saveManager}, gridPtr{newGridPtr}, applicationPtr{application}
{
    try
    {
        Gtk::Window::set_icon(Gdk::Pixbuf::create_from_file(ProgramStuff::GTKSlide::Resources::LOGO));
    }
    catch (...)
    {
        g_warning("Could not open 15Slide logo");//not catostrophic if logo cannot be found
    }

    set_title("15Slide");
    //set_border_width(10);//fixme makes menu bar look weird

    set_resizable(false);
    set_position(Gtk::WIN_POS_CENTER);
    signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::exit));

    add(mainGrid);


    mainMenu = createMenuBar();//create a menu bar
    mainGrid.add(mainMenu);//adds a menu bar to the box

    mainGrid.attach_next_to(tileGrid, mainMenu, Gtk::POS_BOTTOM, 1, 1);

    show_all_children();
}

MainWindow::~MainWindow() {}

Gtk::MenuBar MainWindow::createMenuBar()
{
    Glib::RefPtr<Gio::SimpleActionGroup> actionGroup {Gio::SimpleActionGroup::create()};
    insert_action_group("actionGroup", actionGroup);

    actionGroup->add_action("newGame", sigc::mem_fun(*this, &MainWindow::on_menuBar_newGame));
    applicationPtr->set_accel_for_action("actionGroup.newGame", "<control>n");

    actionGroup->add_action("save", [this]
    {//lambda calls save and does not care about return value
        save();
    });
    applicationPtr->set_accel_for_action("actionGroup.save", "<control>s");

    actionGroup->add_action("saveAs", [this]
    {//lambda calls save and does not care about return value
        saveAs();
    });
    applicationPtr->set_accel_for_action("actionGroup.saveAs", "<control><shift>s");

    actionGroup->add_action("load", sigc::mem_fun(*this, &MainWindow::on_menuBar_load));
    applicationPtr->set_accel_for_action("actionGroup.load", "<control>l");

    actionGroup->add_action("exit", [this]
    {//lambda calls exit function (same one as x button)
        exit(nullptr);
    });
    applicationPtr->set_accel_for_action("actionGroup.exit", "<control>q");

    //actionGroup->add_action("autoSave", sigc::mem_fun(*this, &MainWindow::on_menuBar_autoSave));
    //applicationPtr->set_accel_for_action("actionGroup.autoSave", "<control>a");

    actionGroup->add_action("demo", []
    {//lambda opens 15Slide website in browser
        gtk_show_uri_on_window(nullptr, "https://jzjisawesome.github.io/15Slide/How-to-play", GDK_CURRENT_TIME, nullptr);
    });
    applicationPtr->set_accel_for_action("actionGroup.demo", "F1");

    actionGroup->add_action("about", [this]
    {//lambda creates temporary about dialog and displays it
        (AboutSlide {*this}).display();
    });
    applicationPtr->set_accel_for_action("actionGroup.about", "a");


    Glib::RefPtr<Gtk::Builder> menuBuilder {Gtk::Builder::create()};
    //build the menu
    if constexpr (ProgramStuff::GTKSlide::USE_EXTERNAL_MENUBAR_XML)
        menuBuilder->add_from_file(ProgramStuff::GTKSlide::Resources::MENUBAR_XML);//no try catch; this is a fatal error
    else
    {
        menuBuilder->add_from_string
        (
            "<interface>"
            "   <menu id='menuBar'>"
            "       <submenu>"
            "           <attribute name='label' translatable='yes'>_File</attribute>"//File menu
            "           <section>"
            "               <section>"
            "                   <item>"//New Game
            "                       <attribute name='label' translatable='yes'>_New Game</attribute>"
            "                       <attribute name='action'>actionGroup.newGame</attribute>"
            "                       <attribute name='accel'>&lt;control&gt;n</attribute>"
            "                   </item>"
            "                   <item>"//Save
            "                       <attribute name='label' translatable='yes'>_Save</attribute>"
            "                       <attribute name='action'>actionGroup.save</attribute>"
            "                       <attribute name='accel'>&lt;control&gt;s</attribute>"
            "                   </item>"
            "                   <item>"//Save As
            "                       <attribute name='label' translatable='yes'>_Save As</attribute>"
            "                       <attribute name='action'>actionGroup.saveAs</attribute>"
            "                       <attribute name='accel'>&lt;control&gt;&lt;shift&gt;s</attribute>"
            "                   </item>"
            "                   <item>"//Load
            "                       <attribute name='label' translatable='yes'>_Load</attribute>"
            "                       <attribute name='action'>actionGroup.load</attribute>"
            "                       <attribute name='accel'>&lt;control&gt;l</attribute>"
            "                   </item>"
            "               </section>"
            "               <section>"
            "                   <item>"//Exit
            "                       <attribute name='label' translatable='yes'>_Exit</attribute>"
            "                       <attribute name='action'>actionGroup.exit</attribute>"
            "                       <attribute name='accel'>&lt;control&gt;q</attribute>"
            "                   </item>"
            "               </section>"
            "           </section>"
            "       </submenu>"
            //might make into settings dialog instead
            //"       <submenu>"
            //"           <attribute name='label' translatable='yes'>_Options</attribute>"//Options menu
            //"           <section>"
            //"                   <item>"//Auto Save
            //"                       <attribute name='label' translatable='yes'>_Auto Save</attribute>"
            //"                       <attribute name='action'>actionGroup.autoSave</attribute>"
            //"                       <attribute name='accel'>&lt;control&gt;a</attribute>"
            //"                   </item>"
            //"           </section>"
            //"       </submenu>"
            "       <submenu>"
            "           <attribute name='label' translatable='yes'>_Help</attribute>"//Help menu
            "           <section>"
            "                   <item>"//How to play
            "                       <attribute name='label' translatable='yes'>_How to play</attribute>"
            "                       <attribute name='action'>actionGroup.demo</attribute>"
            "                       <attribute name='accel'>F1</attribute>"
            "                   </item>"
            "                   <item>"//About
            "                       <attribute name='label' translatable='yes'>_About 15Slide</attribute>"
            "                       <attribute name='action'>actionGroup.about</attribute>"
            "                       <attribute name='accel'>a</attribute>"
            "                   </item>"
            "           </section>"
            "       </submenu>"
            "   </menu>"
            "</interface>"
        );
    }

    //return it
    Glib::RefPtr<Glib::Object> menuObject = menuBuilder->get_object("menuBar");
    Glib::RefPtr<Gio::Menu> newMenuBar = Glib::RefPtr<Gio::Menu>::cast_dynamic(menuObject);
    if (!newMenuBar)
        throw std::runtime_error {"Could not create a menu bar"};
    else
        return Gtk::MenuBar {newMenuBar};
}

void MainWindow::on_menuBar_newGame()
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)final touches" << std::endl;


    if (!saveManager->isSaved)
    {
        Gtk::MessageDialog notSavedDialog("Wait!");
        notSavedDialog.set_title("Wait!");

        notSavedDialog.set_secondary_text("What do you want to do with this unsaved grid?");

        //super jankey looking and discouraged way to remove ok button
        notSavedDialog.get_action_area()->remove
        (
            *(notSavedDialog.get_action_area()->get_children()[0])//1st and only element is ok (gone now)
        );

        notSavedDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        notSavedDialog.add_button("_Discard grid", Gtk::RESPONSE_REJECT);
        notSavedDialog.add_button("_Save grid", Gtk::RESPONSE_OK);

        notSavedDialog.set_default_response(Gtk::RESPONSE_CANCEL);

        notSavedDialog.set_transient_for(*this);
        notSavedDialog.show_all();
        notSavedDialog.present();

        switch (notSavedDialog.run())
        {
        case Gtk::RESPONSE_OK:
            save();//no break statement here on purpose
        case Gtk::RESPONSE_REJECT:
        {
            //reset previous save file
            saveManager->saveFile = {""};
            saveManager->isSaved = {false};

            (*gridPtr) = {Grid15::GridHelp::generateRandomGrid()};

            tileGrid.updateTiles();
            break;
        }
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT://x button
        default:
            break;
        }
    }
    else
    {
        saveManager->saveFile = {""};
        saveManager->isSaved = {false};

        (*gridPtr) = {Grid15::GridHelp::generateRandomGrid()};

        tileGrid.updateTiles();
    }
}

bool MainWindow::save()
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)not done" << std::endl;

    if (saveManager->saveFile != "")
    {
        Grid15::GridHelp::save(saveManager->saveFile, *gridPtr);//fixme error handeling needed
        return true;//here too
    }
    else
        return saveAs();
}

bool MainWindow::saveAs()
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)final touches" << std::endl;

    SlideFileDialog saveDialog(*this, "Choose a file to save to", Gtk::FILE_CHOOSER_ACTION_SAVE);

    const int result {saveDialog.run()};

    if (result == Gtk::RESPONSE_OK)
    {
        try
        {
            Grid15::GridHelp::save(saveDialog.get_filename(), *gridPtr);

            //we only get here if the file works
            saveManager->saveFile = {saveDialog.get_filename()};
            saveManager->isSaved = {true};

            tileGrid.updateTiles();

            return true;
        }
        catch (std::ios_base::failure &e)
        {
            saveDialog.hide();//hide the file dialog first

            Gtk::MessageDialog errorDialog("Some this went wrong while saving");
            errorDialog.set_title("Oh no!");

            errorDialog.set_secondary_text("Try a diffrent file/location, or change permissions to allow writing");

            errorDialog.set_transient_for(*this);
            errorDialog.show_all();
            errorDialog.present();
            errorDialog.run();

            return false;
        }
    }
    else
        return false;
}

void MainWindow::on_menuBar_load()
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)final touches" << std::endl;

    if (!saveManager->isSaved)
    {
        Gtk::MessageDialog notSavedDialog("Wait!");
        notSavedDialog.set_title("Wait!");

        notSavedDialog.set_secondary_text("What do you want to do with this unsaved grid?");

        //super jankey looking and discouraged way to remove ok button
        notSavedDialog.get_action_area()->remove
        (
            *(notSavedDialog.get_action_area()->get_children()[0])//1st and only element is ok (gone now)
        );

        notSavedDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        notSavedDialog.add_button("_Discard grid", Gtk::RESPONSE_REJECT);
        notSavedDialog.add_button("_Save grid", Gtk::RESPONSE_OK);

        notSavedDialog.set_default_response(Gtk::RESPONSE_CANCEL);

        notSavedDialog.set_transient_for(*this);
        notSavedDialog.show_all();
        notSavedDialog.present();

        switch (notSavedDialog.run())
        {
        case Gtk::RESPONSE_OK:
            save();//no break statement here on purpose
        case Gtk::RESPONSE_REJECT:
        {
            saveManager->saveFile = {""};
            saveManager->isSaved = {false};

            (*gridPtr) = {Grid15::GridHelp::generateRandomGrid()};

            tileGrid.updateTiles();
            break;
        }
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT://x button
        default:
        {
            return;//instead of continuing to the load dialog, return from the function
            break;
        }
        }
    }

    SlideFileDialog loadDialog(*this, "Choose a file to load from", Gtk::FILE_CHOOSER_ACTION_OPEN);

    if (loadDialog.run() == Gtk::RESPONSE_OK)
    {
        try
        {
            Grid15::GridHelp::load(loadDialog.get_filename(), *gridPtr);

            //we only get here if the file works
            saveManager->saveFile = {loadDialog.get_filename()};
            saveManager->isSaved = {true};

            tileGrid.updateTiles();
        }
        catch (std::ios_base::failure &e)
        {
            loadDialog.hide();//hide the file dialog first

            Gtk::MessageDialog errorDialog("Some this went wrong while loading");
            errorDialog.set_title("Oh no!");

            errorDialog.set_secondary_text("Try a diffrent file/location, change permissions to allow reading, and make sure it is a valid 15Slide save file");

            errorDialog.set_transient_for(*this);
            errorDialog.show_all();
            errorDialog.present();
            errorDialog.run();
        }
        catch (std::invalid_argument &e)
        {
            loadDialog.hide();//hide the file dialog first

            Gtk::MessageDialog errorDialog("Some this went wrong while importing the grid");
            errorDialog.set_title("Oh no!");

            errorDialog.set_secondary_text("The file was read sucessfully, but the grid may be corrupted");

            errorDialog.set_transient_for(*this);
            errorDialog.show_all();
            errorDialog.present();
            errorDialog.run();
        }
    }
}
/*
void MainWindow::on_menuBar_autoSave()
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)to do" << std::endl;
}
*/

bool MainWindow::exit(GdkEventAny* event)
{
    if constexpr (ProgramStuff::Build::DEBUG)
        std::clog << "(debug)final touches" << std::endl;

    if (!saveManager->isSaved)
    {
        Gtk::MessageDialog notSavedDialog("Wait!");
        notSavedDialog.set_title("Wait!");

        notSavedDialog.set_secondary_text("What do you want to do with this unsaved grid?");

        //super jankey looking and discouraged way to remove ok button
        notSavedDialog.get_action_area()->remove
        (
            *(notSavedDialog.get_action_area()->get_children()[0])//1st and only element is ok (gone now)
        );

        notSavedDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        notSavedDialog.add_button("_Discard grid", Gtk::RESPONSE_REJECT);
        notSavedDialog.add_button("_Save grid", Gtk::RESPONSE_OK);

        notSavedDialog.set_default_response(Gtk::RESPONSE_CANCEL);

        notSavedDialog.set_transient_for(*this);
        notSavedDialog.show_all();
        notSavedDialog.present();

        switch (notSavedDialog.run())
        {
        case Gtk::RESPONSE_REJECT:
        {
            hide();
            break;
        }
        case Gtk::RESPONSE_OK://fixme if user presses cancel in next dialog program closes without saving
        {
            if (save())
                hide();//only close the program if the user pressed ok and the file was valid
            break;
        }
        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT://x button
        default:
            break;
        }
    }
    else
        hide();

    return true;
}
}
