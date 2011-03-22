/*
 * config.h
 *
 *  Created on: 2011-02-20
 *      Author: fred
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define panel_count_x (4)
#define panel_count_y (1)

#define panel_count (panel_count_x * panel_count_y)

#define ROWS (8)
#define COLS (8 * panel_count)

#define X_MAX (8 * panel_count_x) // horizontal display size
#define Y_MAX (8 * panel_count_y) // vertical display size

#endif /* CONFIG_H_ */
