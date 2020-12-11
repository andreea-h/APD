package com.apd.tema2.factory;

import com.apd.tema2.entities.Intersection;
import com.apd.tema2.intersections.*;

import java.util.HashMap;
import java.util.Map;

/**
 * Prototype Factory: va puteti crea cate o instanta din fiecare tip de implementare de Intersection.
 */
public class IntersectionFactory {
    private static Map<String, Intersection> cache = new HashMap<>();

    static {
         cache.put("simpleIntersection", new SimpleIntersection());
         cache.put("simple_n_Intersection", new Simple_N_Intersection());
         cache.put("simple_strict_1_car_roundabout", new SimpleStrict_1_CarRoundabout());
         cache.put("simple_strict_x_car_roundabout", new SimpleStrict_X_CarRoundabout());
         cache.put("simple_max_x_car_roundabout", new SimpleMax_X_CarRoundabout());
         cache.put("priority_intersection", new PriorityIntersection());
         cache.put("crosswalk", new CrossWalk());
         cache.put("simple_maintenance", new SimpleMaintenance());
         cache.put("complex_maintenance", new ComplexMaintenance());
         cache.put("railroad", new Railroad());
    }

    public static Intersection getIntersection(String handlerType) {
        return cache.get(handlerType);
    }

}
