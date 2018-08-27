=====================
Reflectometry Changes
=====================

.. contents:: Table of Contents
   :local:

.. warning:: **Developers:** Sort changes under appropriate heading
    putting new features at the top of the section, followed by
    improvements, followed by bug fixes.

Algorithms
----------

New Algorithms
##############

- :ref:`ReflectometryCorrectDetectorAngle <algm-ReflectometryCorrectDetectorAngle>` can be used to rotate the detector to given :math:`2 \theta` angle or correct the detector angle using a direct beam reference.


Liquids Reflectometer
---------------------

- Default x-direction pixel range for the scaling factor calculation is now set to the full width of the detector as opposed to a restricted guess.

ISIS Reflectometry Interface
----------------------------

Bug fixes
#########

- A bug has been fixed on the Settings tab where the IncludePartialBins check box had been hidden by a misplaced text entry box.

:ref:`Release 3.14.0 <v3.14.0>`
