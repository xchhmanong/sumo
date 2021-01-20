/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEMoveFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// The Widget for move elements
/****************************************************************************/
#pragma once

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEMoveFrame
 * The Widget for create edges
 */
class GNEMoveFrame : public GNEFrame {

public:

    // ===========================================================================
    // class NetworkModeOptions
    // ===========================================================================

    class NetworkModeOptions : protected FXGroupBox {

    public:
        /// @brief constructor
        NetworkModeOptions(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~NetworkModeOptions();

        /// @brief move whole polygons
        bool getMoveWholePolygons() const;

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief checkbox for enable/disable move whole polygons
        FXCheckButton* myMoveWholePolygons;
    };

    // ===========================================================================
    // class ShiftEdgeGeometry
    // ===========================================================================

    class ShiftEdgeGeometry : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::ShiftEdgeGeometry)

    public:
        /// @brief constructor
        ShiftEdgeGeometry(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~ShiftEdgeGeometry();

        /// @brief show shift edge geometry
        void showShiftEdgeGeometry();

        /// @brief hide change Z in selection
        void hideShiftEdgeGeometry();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes Z value
        long onCmdChangeShiftValue(FXObject*, FXSelector, void*);

        /// @brief Called when user press the apply Z value button
        long onCmdShiftEdgeGeometry(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ShiftEdgeGeometry)

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief textField for shift value
        FXTextField* myShiftValueTextField = nullptr;
    };

    // ===========================================================================
    // class ChangeZInSelection
    // ===========================================================================

    class ChangeZInSelection : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::ChangeZInSelection)

    public:
        /// @brief constructor
        ChangeZInSelection(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~ChangeZInSelection();

        /// @brief show change Z in selection
        void showChangeZInSelection();

        /// @brief hide change Z in selection
        void hideChangeZInSelection();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes Z value
        long onCmdChangeZValue(FXObject*, FXSelector, void*);

        /// @brief Called when user changes Z mode
        long onCmdChangeZMode(FXObject*, FXSelector, void*);

        /// @brief Called when user press the apply Z value button
        long onCmdApplyZ(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ChangeZInSelection)

        /// @brief update label
        void updateInfoLabel();

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame *myMoveFrameParent;

        /// @brief textField for Z value
        FXTextField* myZValueTextField = nullptr;
        
        /// @brief radio button for absolute value
        FXRadioButton* myAbsoluteValue = nullptr;

        /// @brief radio button for relative value
        FXRadioButton* myRelativeValue = nullptr;

        /// @brief info label
        FXLabel *myInfoLabel = nullptr;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEMoveFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEMoveFrame();

    /**@brief handle processClick and set the relative colouring
     * @param[in] clickedPosition clicked position over ViewNet
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @param objectsUnderGrippedCursor collection of objects under gripped cursor after click over view
     */
    void processClick(const Position& clickedPosition,
                      const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
                      const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderGrippedCursor);

    /// @brief show prohibition frame
    void show();

    /// @brief hide prohibition frame
    void hide();

    /// @brief get network mode options
    NetworkModeOptions* getNetworkModeOptions() const;

private:
    /// @brief modul for NetworkMode Options
    NetworkModeOptions *myNetworkModeOptions = nullptr;

    /// @brief modul for shift edge geometry
    ShiftEdgeGeometry *myShiftEdgeGeometry = nullptr;

    /// @brief modul for change Z in selection
    ChangeZInSelection* myChangeZInSelection = nullptr;
};
