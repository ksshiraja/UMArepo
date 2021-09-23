/*
 * Copyright 2019 UM::Autonomy
 */

#ifndef UMA_NAVIGATION_PATH_H
#define UMA_NAVIGATION_PATH_H

#include <lib/errors/errors.h>
#include <uma_navigation/waypoint.h>

#include <deque>
#include <string>
#include <cstddef>
#include <cmath>
#include <costmap_2d/costmap_2d.h>
#include <vector>


// custom exception to be used by the path class
class PathException : public uma::UMAException
{
public:
    explicit PathException(const std::string &msg):
        uma::UMAException(msg)
    {}
};

enum Side
{
    LEFT = 0,
    RIGHT = 1,
    TOP = 2,
    BOTTOM = 3
};

/*
 * Class Path:
 * This class holds information about our current path_ variable
 * represents the path generated by theta*, which is used to 
 * overlay a spline for the spline_path_, which is sent to controls
 */
class Path
{
public:
    using iterator = std::deque<Waypoint>::iterator;
    using const_iterator = std::deque<Waypoint>::const_iterator;
    using vec_const_iterator = std::vector<Coordinate<double> >::const_iterator;

    explicit Path(const costmap_2d::Costmap2D *grid_in);

    explicit Path(Path * p);

    /*
     * Check whether the current path is still valid given the current costmap
     * (ie check whether path intersects an obstacle)
     */
    bool isSplinePathValid() const;

    /*
     * Add waypoint to the end of the path
     */
    void addWaypoint(const Waypoint &wp);

    /*
     * Add waypoint in the middle of our path
     * 
     * Returns: whether the insertion was successful
     */
    bool insertWaypoint(const Waypoint& wp, size_t index);

    /*
     * Remove a waypoint from the path
     * 
     * Returns: whether the removal was successful
     */
    bool removeWaypoint(const Waypoint &wp);
    bool removeWaypoint(size_t index);

    /*
     * Removes all waypoints from the current path
     */
    void clearPath();

    /*
     * Gets path (pre-spline version)
     */
    const std::deque<Waypoint>& getPath() const;

    const std::vector<Coordinate<double> >* getSplinePath() const;

    const_iterator begin() const
    {
        return path_.begin();
    }

    const_iterator end() const
    {
        return path_.end();
    }

    /*
    * We currently have a path of points generated by theta* that are not necessarily
    * equidistant from each other. This overlays a spline onto this path using a spline library.
    * The spline path has equidistant points to allow motion planning to hit all the points smoothly.
    */
    void interpolateSpline();

    /*
    * We don't want to add a spline, but we want the path to be the same format as a spline path.
    * So we fill in between the waypoints with a straight line of equidistant points with the 
    * same density of a spline path.
    */
    void noSplinePath();

    size_t getSize();

private:
    // iterates through each segment and returns whether the whole path is valid
    bool isSplinePathValidPrivate(vec_const_iterator begin, vec_const_iterator end) const;
    // Because you can't express a spline with an equation, we instead approximate a line
    // by having a vector of a densly packed coordintes.
    std::vector<Coordinate<double> > spline_path_;
    std::deque<Waypoint> path_;
    const costmap_2d::Costmap2D *grid_;
};

#endif  // UMA_NAVIGATION_PATH_H