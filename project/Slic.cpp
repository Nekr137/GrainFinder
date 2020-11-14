#include "Slic.h"
#include "ImageRGB.h"
#include <iostream>

namespace SLIC
{

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
				Cluster cl(r, g, b, (double)i, (double)j);
				_aClusters.push_back(cl);

				// to do: Move
			}
		}

		const size_t slicIterationsCnt = 4;
		for (size_t iter = 0; iter < 4; ++iter) {

			std::cout << "\n [ SLIC ] iter: " << iter << " of " << slicIterationsCnt;
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

			// Remove weird clusters
			for (size_t k = _aClusters.size(); k > 0; --k) {

				// Small grains
				bool remove = false;

				remove |= _aClusters[k - 1]._aPixels.size() < 500;
				//remove |= _aClusters[k - 1].

				if (remove)
					_aClusters.erase(_aClusters.begin() + k - 1);

			}
		}

		return true;
	}

	void Slic::Show() const {

		std::cout << "\n [ SLIC ] Showing an output";

		// 2D array of cluster indices _imgW x _imgH
		std::vector<std::vector<size_t>> aaCI(_imgW, std::vector<size_t>(_imgH));

		for (size_t clIdx = 0; clIdx < _aClusters.size(); ++clIdx) {
			const Cluster& cl = _aClusters[clIdx];
			for (size_t i = 0; i < cl._aPixels.size(); ++i) {
				const P2D px = cl._aPixels[i];
				aaCI[px.x][px.y] = clIdx;
			}
		}

		Channel ch(_imgW * 2, _imgH);
		ImageRGB im(ch, ch, ch);

		for (size_t i = 0; i < _imgW; ++i) {
			for (size_t j = 0; j < _imgH; ++j) {
				double r, g, b;
				_pImage->Get(i, j, r, g, b);
				im.Set(i, j, r, g, b);
			}
		}

		for (const Cluster& cl : _aClusters) {
			im.Set((size_t)cl.x, (size_t)cl.y, 1.0, 0.0, 0.0);
			if (cl.x > 0) im.Set((size_t)(cl.x - 1), (size_t)cl.y, 1.0, 0.0, 0.0);
			if (cl.y > 0) im.Set((size_t)cl.x, (size_t)(cl.y - 1), 1.0, 0.0, 0.0);
		}

		for (size_t i = 0; i < _imgW; ++i){
			for (size_t j = 0; j < _imgH; ++j){

				if (
					(i > 0 && aaCI[i][j] != aaCI[i - 1][j]) ||
					(i + 1 < _imgW && aaCI[i][j] != aaCI[i + 1][j]) ||
					(j > 0 && aaCI[i][j] != aaCI[i][j - 1]) ||
					(j + 1 < _imgH && aaCI[i][j] != aaCI[i][j + 1])
					) {
					im.Set(i, j, 1.0, 0.0, 0.0);
				}
			}
		}

		size_t nClusters = _aClusters.size();
		std::cout << "\nClusters: " << nClusters;

		auto showGrains = [&]() {
			size_t i = 0;
			for (double r = 0.0; r < 1.0 + 1e-5; r += 0.2) {
				for (double g = 0.0; g < 1.0 + 1e-5; g += 0.2) {
					for (double b = 0.0; b < 1.0 + 1e-5; b += 0.2) {

						if (i == nClusters)
							return;

						for (const auto& px : _aClusters[i]._aPixels) {
							im.Set(px.x + _imgW, px.y, r, g, b);
						}

						i++;
					}
				}
			}
		};

		showGrains();
		im.Show();
	}

	bool Cluster::IsBorderPixel(size_t pxlIdx) const {
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

};