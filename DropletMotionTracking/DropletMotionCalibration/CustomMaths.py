#
# FROM : http://wiki.scipy.org/Cookbook/Least_Squares_Circle
#
#  == METHOD 2 ==
import numpy as np
from scipy import optimize

def calc_R(cX, cY, x, y):
    """ calculate the distance of each 2D points from the center (xc, yc) """
    return np.sqrt((x-cX)**2 + (y-cY)**2)

def f_2(x, y):
    """ calculate the algebraic distance between the data points and the mean circle centered at c=(xc, yc) """
    def f2_helper(c):
        Ri = calc_R(c[0],c[1], x, y)
        return Ri - Ri.mean()
    return f2_helper;

'''
Function below from 3D line intersection algorithm from the article "Intersection of two lines in three-space" by Ronald Goldman. Via Stack Overflow.
'''
def line_intersect(a1, a2, b1, b2):
    p = a1
    r = a2-a1
    q = b1
    s = b2-b1
    u_numer = np.cross((q-p),r)
    t_numer = np.cross((q-p),s)
    denom = np.cross(r, s)
    if abs(denom)<0.00001:
        if abs(u_numer)<0.00001:
            print("Colinear! This shouldn't happen?")
            return
        else:
            return
    else:
        u = u_numer/denom
        t = t_numer/denom
        if (0 <= t) and (t <= 1) and (0 <= u) and (u <= 1):
            intersectA = p + t*r
            intersectB = q + u*s
            return intersectA
        else:
            return

def pick_direction(pos, theta, bases, x_boundaries, y_boundaries):
    print("In pick_direction.")
    rotation_matrix = np.array([[np.cos(theta), -np.sin(theta)],[np.sin(theta), np.cos(theta)]])
    print("\tRotation Matrix:\n\t%s"%(str(rotation_matrix)))
    print("\tBases:\n\t%s"%(str(bases)))
    rotated_bases = [rotation_matrix.dot(basis) for basis in bases]
    print("\tRotated Bases:\n\t%s"%(str(rotated_bases)))
    # print(rotated_bases)

    i=1
    while pos[0] >= x_boundaries[i]:
        i+=1
    temp_x = pos[0] - x_boundaries[i-1]
    x_bound = x_boundaries[i]-x_boundaries[i-1]

    j=1 
    while pos[1] >= y_boundaries[j]:
        j+=1
    temp_y = pos[1] - y_boundaries[j-1]
    y_bound = y_boundaries[j]-y_boundaries[j-1]

    temp_pos = np.array([temp_x, temp_y])
    temp_bounds = np.array([x_bound, y_bound])
    #  print("temp_pos: (%d, %d), temp_bounds: (%d, %d)"%(temp_pos[0], temp_pos[1], temp_bounds[0], temp_bounds[1]))

    line_segments = [[temp_pos, temp_pos+1000.0*rotated_bases[i]] for i in range(3)] #positive directions
    line_segments.extend([[temp_pos, temp_pos-1000.0*rotated_bases[i]] for i in range(3)]) #negative directions
    #now line_sigments[i] is the line segment going from center of droplet to far away in direction i.

    boundary_line_segments = [[np.array([0,0]), np.array([x_bound, 0])], [np.array([0,0]), np.array([0, y_bound])],
                                [np.array([x_bound, 0]), np.array([x_bound, y_bound])], [np.array([0,y_bound]), np.array([x_bound, y_bound])]]

    max_dist = 0
    max_dir = -1
    #print("\tIntersections:")
    for dir in range(6):
        for boundary in boundary_line_segments:
            blah = tuple(map(str, [line_segments[dir][0], line_segments[dir][1], boundary[0], boundary[1]]))
            intersect = line_intersect(line_segments[dir][0], line_segments[dir][1], boundary[0], boundary[1])
            if (intersect is None):
                continue
            dist = np.linalg.norm(intersect-temp_pos)
            #print("\t\t%s, dist: %f"%(str(intersect), dist))
            if dist>max_dist:
                max_dist = dist
                max_dir = dir
    return max_dir

def lsq(positions):
    x, y = positions.transpose()
    x_m = x.mean()
    y_m = y.mean()
    center_estimate = x_m, y_m
    calc_center, ier = optimize.leastsq(f_2(x,y), center_estimate)

    ccX, ccY = calc_center
    calc_Ri       = calc_R(ccX, ccY, x, y)
    radius        = calc_Ri.mean()
    residual   = np.sum((calc_Ri - radius)**2)

    center=np.array(calc_center)

    first_pos = positions[0]
    last_pos = positions[-1]
    sign = np.linalg.det(np.array([last_pos-first_pos, center-first_pos]).transpose())
 
#   print("center: (%f,%f), rad: %f, residu: %f"%(ccX,ccY,radius,residual))
    return (center, radius, residual, sign)