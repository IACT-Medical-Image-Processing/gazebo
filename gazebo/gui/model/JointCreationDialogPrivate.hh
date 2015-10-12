/*
 * Copyright (C) 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef _GAZEBO_JOINT_CREATION_DIALOG_PRIVATE_HH_
#define _GAZEBO_JOINT_CREATION_DIALOG_PRIVATE_HH_

#include <string>

#include "gazebo/gui/qt.h"

namespace gazebo
{
  namespace gui
  {
    /// \brief Private data for the JointCreationDialog class
    class JointCreationDialogPrivate
    {
      /// \brief Config widget for configuring joint properties.
      public: ConfigWidget *configWidget;

      /// \brief Widget for the parent link.
      public: ConfigChildWidget *parentLinkWidget;

      /// \brief Widget for the child link.
      public: ConfigChildWidget *childLinkWidget;

      /// \brief Pointer to the joint maker.
      public: JointMaker *jointMaker;

      /// \brief Group of buttons for joint types.
      public: QButtonGroup *typeButtons;

      /// \brief Axis presets combo box.
      public: QComboBox *axis1PresetsCombo;

      /// \brief Axis presets combo box.
      public: QComboBox *axis2PresetsCombo;

      /// \brief A list of gui editor events connected to this palette.
      public: std::vector<event::ConnectionPtr> connections;

      /// \brief Button to create joint.
      public: QPushButton *createButton;

      /// \brief Button to swap parent and child links.
      public: QToolButton *swapButton;

      /// \brief Style sheet for link widget which is currently active.
      public: QString activeStyleSheet;

      /// \brief Style sheet for link widgets when there's a warning.
      public: QString warningStyleSheet;

      /// \brief Normal style sheet for link widgets.
      public: QString normalStyleSheet;

      public: QToolButton *xAlignMin;
      public: QToolButton *xAlignCenter;
      public: QToolButton *xAlignMax;
      public: QToolButton *yAlignMin;
      public: QToolButton *yAlignCenter;
      public: QToolButton *yAlignMax;
      public: QToolButton *zAlignMin;
      public: QToolButton *zAlignCenter;
      public: QToolButton *zAlignMax;

    };
  }
}
#endif
