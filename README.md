# QCoverage README

## What does it do?

It is a simple Qt GUI application, that helps to visualize graph coverage criteria (in software testing).

It may be useful for courses based on `"Introduction to Software Testing", Ammann and Offutt`.

The interface should be self-explanatory. You create a graph, then the app computes requirements and coverage
for it using selected criteria.

(Don't forget to try right-clicking a node.)

Generally speaking it is a bit friendlier version of (this webapp)[http://cs.gmu.edu:8080/offutt/coverage/GraphCoverage].

DISCLAIMER: I don't have any affiliation with the authors.

## Caveat

The prime path coverage seems to be different than the one produced by other algorithms. From what I've learned it seems
that the coverage is correct but the prime path criteria are not unambiguous thus diferrent algorithms are possible.

Keep in mind that the algorithm was developed overnight and it may contain bugs.

## How to build the app?

The app was tested with Qt4. In the main directory run `qmake` then `make`.

## LICENSE

Copyright &copy; 2010 Filip Sobalski <pinkeen@gmail.com>
QCoverage - an application for demonstrating graph coverage criteria

This file is part of QCoverage.

QCoverage is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
