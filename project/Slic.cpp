#include "Slic.h"
#include "ImageRGB.h"
#include <iostream>


Slic::Slic(const ImageRGB* ipImage)
	: _pImage(ipImage) {

	_imgW = _pImage->GetWidth();
	_imgH = _pImage->GetHeight();
}

double Slic::ClusterToPixelDist(const Cluster& iCl, const P2D& iPixel) {

	double r, g, b;
	_pImage->Get(iPixel.x, iPixel.y, r, g, b);

	double ll = iCl.l - r;
	double aa = iCl.a - g;
	double bb = iCl.b - b;

	double dlab = std::sqrt(ll*ll + aa*aa + bb*bb);
	double xx = iCl.x - iPixel.x;
	double yy = iCl.y - iPixel.y;

	double dxy = std::sqrt(xx*xx + yy*yy);

	double ds = dlab + _m / _S * dxy;

	return ds;
}

bool Slic::Perform(const size_t K, const double m) {

	std::cout << "\n [ SLIC ] Performing a SLIC clasterization";

	_m = m;
	size_t N = _imgW * _imgH;
	_S = std::sqrt(N / K);

	// Init clusters
	for (double h = _S;; h += _S) {

		if ((size_t)h > _imgH - _S)
			break;

		for (double w = _S;; w += _S) {
		
			if ((size_t)w > _imgW - _S)
				break;

			double r, g, b;

			size_t i = static_cast<size_t>(w);
			size_t j = static_cast<size_t>(h);

			_pImage->Get(i, j, r, g, b);
			Cluster cl(r, g, b, (double) i, (double) j);
			_aClusters.push_back(cl);

			// to do: Move
		}
	}

	for (size_t iter = 0; iter < 3; ++iter) {

		std::cout << "\n [ SLIC ] iter: " << iter;
		// 
		for (auto& cl : _aClusters)
			cl._aPixels.clear();

		// 
		for (size_t i = 0; i < _imgW; ++i) {
			for (size_t j = 0; j < _imgH; ++j) {

				P2D pxl(i, j);
				double minDist = 1e100;
				size_t nearestClusterIdx = 0;

				for (size_t k = 0; k < _aClusters.size(); ++k) {

					Cluster& cl = _aClusters[k];

					double dist = ClusterToPixelDist(cl, pxl);
					if (dist < minDist) {
						minDist = dist;
						nearestClusterIdx = k;
					}
				}
				_aClusters[nearestClusterIdx]._aPixels.push_back(pxl);
			}
		}

		// Find cluster centers
		for (size_t k = 0; k < _aClusters.size(); ++k) {

			Cluster& cl = _aClusters[k];
			double sumR = 0.0, sumG = 0.0, sumB = 0.0, x = 0.0, y = 0.0;
			double nPixels = static_cast<double>(cl._aPixels.size());

			for (const P2D px : cl._aPixels) {

				size_t i = static_cast<size_t>(px.x);
				size_t j = static_cast<size_t>(px.y);

				double r, g, b;
				_pImage->Get(i, j, r, g, b);

				x += px.x;
				y += px.y;
				sumR += r;
				sumG += g;
				sumB += b;
			}

			cl.x = x / nPixels;
			cl.y = y / nPixels;
			cl.l = sumR / nPixels;
			cl.a = sumG / nPixels;
			cl.b = sumB / nPixels;
		}
		Show();
	}

	return true;
}

void Slic::Show() const {

	std::cout << "\n [ SLIC ] Showing an intermediate output";

	ImageRGB im(
		_pImage->GetRedChannelRef().Copy(), 
		_pImage->GetGreenChannelRef().Copy(),
		_pImage->GetBlueChannelRef().Copy()
		);

	for (const Cluster& cl : _aClusters) {
		im.Set(cl.x, cl.y, 1.0, 0.0, 0.0);
	}

	for (const Cluster& cl : _aClusters) {
		for (size_t i = 0; i < cl._aPixels.size(); ++i) {
			P2D px = cl._aPixels[i];
			if (cl.IsBorderPixel(i)) {
				im.Set(px.x, px.y, 0.0, 0.0, 0.0);
			}
		}
	}

	im.Show();
}

bool Slic::Cluster::IsBorderPixel(size_t pxlIdx) const {
	P2D px = _aPixels[pxlIdx];
	size_t neighbCnt = 0;
	for (size_t j = 0; j < _aPixels.size(); ++j) {
		if (neighbCnt == 4)
			break;
		if (pxlIdx == j)
			continue;
		if (px.y == _aPixels[j].y) {
			if (px.x == _aPixels[j].x - 1 || px.x == _aPixels[j].x + 1)
				neighbCnt++;
		}
		if (px.x == _aPixels[j].x) {
			if (px.y == _aPixels[j].y - 1 || px.y == _aPixels[j].y + 1)
				neighbCnt++;
		}
	}
	return neighbCnt < 4;
}

//
//void Slic::Init(int step, int nc) {
//
//	this->step = step;
//	this->nc = nc;
//	this->ns = step;
//
//	for (int i = 0; i < _imgW; i++) {
//
//		std::vector<int> cr;
//		std::vector<double> dr;
//
//		for (int j = 0; j < _imgH; j++) {
//			cr.push_back(-1);
//			dr.push_back(DBL_MAX);
//		}
//		_clusters.push_back(cr);
//		_distances.push_back(dr);
//	}
//
//	for (int i = step; i < _imgW - step / 2; i += step) {
//		for (int j = step; j < _imgH - step / 2; j += step) {
//			std::vector<double> center;
//			P2D locMin = FindLocalMinimum(P2D(i, j));
//			double br = _ipChannel->Get(locMin.x, locMin.y);
//
//			center.push_back(br);
//			center.push_back((double)locMin.x);
//			center.push_back((double)locMin.y);
//
//			_centers.push_back(center);
//			_nCenters.push_back(0);
//		}
//	}
//}
//
//void Slic::Generate() {
//
//	for (int i = 0; i < 10; i++) {
//		for (int j = 0; j < _imgW; j++) {
//			for (int k = 0; k < _imgH; k++) {
//				_distances[j][k] = FLT_MAX;
//			}
//		}
//
//		for (int j = 0; j < (int)_centers.size(); j++) {
//			/* Only compare to pixels in a 2 x step by 2 x step region. */
//			for (int k = _centers[j][1] - step; k < _centers[j][1] + step; k++) {
//				for (int l = _centers[j][2] - step; l < _centers[j][2] + step; l++) {
//
//					if (k >= 0 && k < _imgW && l >= 0 && l < _imgH) {
//						double color = _ipChannel->Get(k,l);
//
//						// Find the distance
//						double dc = std::sqrt(pow(_centers[j][0] - color, 2));
//						double ds = std::sqrt(pow(_centers[j][1] - k, 2) + pow(_centers[j][2] - l, 2));
//						double d = std::sqrt(std::pow(dc / static_cast<double>(nc), 2) + std::pow(ds / static_cast<double>(ns), 2));
//
//						/* Update cluster allocation if the cluster minimizes the
//						distance. */
//						if (d < _distances[k][l]) {
//							_distances[k][l] = d;
//							_clusters[k][l] = j;
//						}
//					}
//				}
//			}
//		}
//
//		/* Clear the center values. */
//		for (int j = 0; j < (int)_centers.size(); j++) {
//			_centers[j][0] = _centers[j][1] = _centers[j][2] = 0;
//			_nCenters[j] = 0;
//		}
//
//		/* Compute the new cluster centers. */
//		for (int j = 0; j < _imgW; j++) {
//			for (int k = 0; k < _imgH; k++) {
//				int c_id = _clusters[j][k];
//
//				if (c_id != -1) {
//					double colour = _ipChannel->Get(j,k);
//
//					_centers[c_id][0] += colour;
//					_centers[c_id][1] += j;
//					_centers[c_id][2] += k;
//
//					_nCenters[c_id] += 1;
//				}
//			}
//		}
//
//		/* Normalize the clusters. */
//		for (int j = 0; j < (int)_nCenters.size(); j++) {
//			_centers[j][0] /= _nCenters[j];
//			_centers[j][1] /= _nCenters[j];
//			_centers[j][2] /= _nCenters[j];
//		}
//	}
//}
//
//void Slic::CreateConnectivity() {
//	int label = 0, adjlabel = 0;
//	const int lims = (_imgW * _imgH) / ((int)_nCenters.size());
//
//	const int dx4[4] = { -1, 0, 1, 0 };
//	const int dy4[4] = { 0, -1, 0, 1 };
//
//	/* Initialize the new cluster matrix. */
//	std::vector<std::vector<int>> new_clusters;
//	for (int i = 0; i < _imgW; i++) {
//		std::vector<int> nc;
//		for (int j = 0; j < _imgH; j++) {
//			nc.push_back(-1);
//		}
//		new_clusters.push_back(nc);
//	}
//
//	for (int i = 0; i < _imgW; i++) {
//		for (int j = 0; j < _imgH; j++) {
//			if (new_clusters[i][j] == -1) {
//				std::vector<P2D> elements;
//				elements.push_back(P2D(i,j));
//
//				/* Find an adjacent label, for possible use later. */
//				for (int k = 0; k < 4; k++) {
//					int x = elements[0].x + dx4[k], y = elements[0].y + dy4[k];
//
//					if (x >= 0 && x < _imgW && y >= 0 && y < _imgH) {
//						if (new_clusters[x][y] >= 0) {
//							adjlabel = new_clusters[x][y];
//						}
//					}
//				}
//
//				int count = 1;
//				for (int c = 0; c < count; c++) {
//					for (int k = 0; k < 4; k++) {
//						int x = elements[c].x + dx4[k], y = elements[c].y + dy4[k];
//
//						if (x >= 0 && x < _imgW && y >= 0 && y < _imgH) {
//							if (new_clusters[x][y] == -1 && _clusters[i][j] == _clusters[x][y]) {
//								elements.push_back(P2D(i,j));
//								new_clusters[x][y] = label;
//								count += 1;
//							}
//						}
//					}
//				}
//
//				/* Use the earlier found adjacent label if a segment size is
//				smaller than a limit. */
//				if (count <= lims >> 2) {
//					for (int c = 0; c < count; c++) {
//						new_clusters[elements[c].x][elements[c].y] = adjlabel;
//					}
//					label -= 1;
//				}
//				label += 1;
//			}
//		}
//	}
//}
//
//Slic::P2D Slic::FindLocalMinimum(const P2D iCenter) {
//
//	double min_grad = DBL_MAX;
//	Slic::P2D locMin(iCenter.x, iCenter.y);
//
//	for (int i = iCenter.x - 1; i < iCenter.x + 2; i++) {
//		for (int j = iCenter.y - 1; j < iCenter.y + 2; j++) {
//			double c1 = _ipChannel->Get(i,j+1);
//			double c2 = _ipChannel->Get(i+1,j);
//			double c3 = _ipChannel->Get(i,j);
//
//			double dx = c1 - c3;
//			double dy = c2 - c3;
//
//			double grad = dx*dx + dy*dy;
//
//			if (grad < min_grad) {
//				min_grad = grad;
//				locMin = P2D(i, j);
//			}
//		}
//	}
//
//	return locMin;
//}
//
//Channel Slic::Borders() {
//
//	const int dx8[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
//	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
//
//	std::vector<P2D> contours;
//	std::vector<std::vector<bool>> istaken;
//	for (int i = 0; i < _imgW; i++) {
//		std::vector<bool> nb;
//		for (int j = 0; j < _imgH; j++) {
//			nb.push_back(false);
//		}
//		istaken.push_back(nb);
//	}
//
//	/* Go through all the pixels. */
//	for (int i = 0; i < _imgW; i++) {
//		for (int j = 0; j < _imgH; j++) {
//			int nr_p = 0;
//
//			/* Compare the pixel to its 8 neighbours. */
//			for (int k = 0; k < 8; k++) {
//				int x = i + dx8[k], y = j + dy8[k];
//
//				if (x >= 0 && x < _imgW && y >= 0 && y < _imgH) {
//					if (istaken[x][y] == false && _clusters[i][j] != _clusters[x][y]) {
//						nr_p += 1;
//					}
//				}
//			}
//
//			/* Add the pixel to the contour list if desired. */
//			if (nr_p >= 2) {
//				contours.push_back(P2D(i, j));
//				istaken[i][j] = true;
//			}
//		}
//	}
//
//	/* Draw the contour pixels. */
//	ImageRGB im(_ipChannel->Copy(), _ipChannel->Copy(), _ipChannel->Copy());
//	Channel copy(_imgW, _imgH);
//	for (int i = 0; i < (int)contours.size(); i++) {
//		copy.Set(contours[i].x, contours[i].y, 1.0);
//		im.Set(contours[i].x, contours[i].y, 1.0, 0.0, 0.0);
//	}
//	im.Show("borders");
//	return copy;
//}
