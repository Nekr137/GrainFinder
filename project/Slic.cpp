#include "Slic.h"
#include "SlicUtils.h"
#include "ImageRGB.h"
#include <iostream>
#include <algorithm>


namespace SLIC
{
	Slic::Slic(const ImageRGB* ipImage)
		: _pImage(ipImage) {

		_imgW = _pImage->GetWidth();
		_imgH = _pImage->GetHeight();

		_pixelClusterIndices = std::vector<size_t>(_imgW * _imgH);
		_pixelClusterBestDistances = std::vector<double>(_imgW * _imgH);
	}

	double Slic::FindColorDist(const Cluster& iCl, const P2D& iPixel) {
		double r, g, b;
		_pImage->Get(iPixel.x, iPixel.y, r, g, b);
		double ll = iCl.l - r;
		double aa = iCl.a - g;
		double bb = iCl.b - b;
		double dlab = std::sqrt(ll*ll + aa*aa + bb*bb);
		return dlab;
	}

	double Slic::FindEuqlDist(const Cluster& iCl, const P2D& iPixel) {
		double xx = iCl.x - iPixel.x;
		double yy = iCl.y - iPixel.y;
		double dxy = std::sqrt(xx*xx + yy*yy);
		return dxy;
	}

	double Slic::ClusterToPixelDist(const Cluster& iCl, const P2D& iPixel) {
		double dlab = FindColorDist(iCl, iPixel);
		double dxy = FindEuqlDist(iCl, iPixel);
		double ds = dlab     +     _m / _S * dxy;
		return ds;
	}

	bool Slic::Perform(const size_t K, const double m) {

		std::cout << "\n [ SLIC ] Performing a SLIC clasterization";

		_m = m;
		size_t N = _imgW * _imgH;
		_S = static_cast<size_t>(std::round(std::sqrt(N / K)));

		std::cout << "\n [ SLIC ] N: " << N;
		std::cout << "\n [ SLIC ] K: " << K;
		std::cout << "\n [ SLIC ] S = sqrt(N/k): " << _S;
		std::cout << "\n [ SLIC ] imgW: " << _imgW;
		std::cout << "\n [ SLIC ] imgH: " << _imgH;
		std::cout << "\n [ SLIC ] imgW/S: " << _imgW / _S;

		// Init clusters

		for (size_t h = _S; h <= _imgH - _S; h += _S) {
			for (size_t w = _S; w <= _imgW - _S; w += _S) {

				double r, g, b;

				size_t i = static_cast<size_t>(std::round(w));
				size_t j = static_cast<size_t>(std::round(h));

				_pImage->Get(i, j, r, g, b);
				Cluster cl(r, g, b, w, h, _pImage);
				_aClusters.push_back(cl);

				// to do: Move
			}
		}

		const size_t slicIterationsCnt = 4;
		for (size_t iter = 0; iter < 4; ++iter) {

			size_t nClusters = _aClusters.size();
			std::vector<double> distances(_imgW * _imgH * nClusters-1, 1e100);

			std::cout << "\n [ SLIC ] iter: " << iter << " of " << slicIterationsCnt;

			// x, y, clusterIdx
			std::cout << "\n [ SLIC ]\t Find all distances from pixels to clusters";
			for (size_t k = 0; k < _aClusters.size(); ++k) {

				const Cluster& cl = _aClusters[k];
				size_t stX, stY, enX, enY;
				FindClusterBorders(cl, stX, enX, stY, enY);

				for (size_t i = stX; i < enX; ++i) {
					for (size_t j = stY; j < enY; ++j) {
						const P2D px(i, j);
						auto distIdx = i + j * _imgW + k * _imgW * _imgH;
						distances[distIdx] = ClusterToPixelDist(cl, px);
					}
				}
			}

			std::cout << "\n [ SLIC ]\t Compare all distances";
			for (size_t i = 0; i < N; ++i)
				_pixelClusterBestDistances[i] = 1e100;

			for (size_t k = 0; k < _aClusters.size(); ++k) {

				const Cluster& cl = _aClusters[k];
				size_t stX, stY, enX, enY;
				FindClusterBorders(cl, stX, enX, stY, enY);

				for (size_t i = stX; i < enX; ++i) {
					for (size_t j = stY; j < enY; ++j) {
						size_t distIdx = i + j * _imgW + k * _imgW * _imgH;
						double dist = distances[distIdx];
						if (dist < _pixelClusterBestDistances[i + j * _imgW]) {
							_pixelClusterBestDistances[i + j * _imgW] = dist;
							_pixelClusterIndices[i + j * _imgW] = k;
						}
					}
				}
			}

			// Store pixels in each cluster

			for (Cluster& cl : _aClusters)
				cl._aPixels.clear();

			for (size_t i = 0; i < _imgW; ++i) {
				for (size_t j = 0; j < _imgH; ++j) {
					size_t clIdx = _pixelClusterIndices[i + j * _imgW];
					_aClusters[clIdx]._aPixels.push_back(P2D(i, j));
				}
			}

			std::cout << "\n [ SLIC ]\t Find cluster centers";
			for (size_t k = 0; k < _aClusters.size(); ++k) {

				Cluster& cl = _aClusters[k];
				double sumR = 0.0, sumG = 0.0, sumB = 0.0, x = 0.0, y = 0.0;
				double nPixels = static_cast<double>(cl._aPixels.size());

				for (const P2D px : cl._aPixels) {

					double r, g, b;
					_pImage->Get(px.x, px.y, r, g, b);

					x += static_cast<double>(px.x);
					y += static_cast<double>(px.y);
					sumR += r;
					sumG += g;
					sumB += b;
				}

				cl.x = x / nPixels;
				cl.y = y / nPixels;
				cl.sx = static_cast<size_t>(std::round(cl.x));
				cl.sy = static_cast<size_t>(std::round(cl.y));
				cl.l = sumR / nPixels;
				cl.a = sumG / nPixels;
				cl.b = sumB / nPixels;
			}
		}

		// Remove empty clusters
		for (size_t i = _aClusters.size(); i > 0; --i) {
			if (_aClusters[i - 1]._aPixels.empty()) {
				_aClusters.erase(_aClusters.begin() + i - 1);
				std::cout << "\nEmpty";
			}
		}

		// Write clusters info
		//std::cout << "\nClusters:";
		//for (const auto& cl : _aClusters) {
		//	double mean, sigma;
		//	cl.FindClusterColorParams(mean, sigma);
		//	std::cout << "\ncluster: " << mean << " " << sigma;
		//}

		return true;
	}

	void Slic::FindClusterBorders(const Cluster& iCLuster, size_t& oLeft, size_t& oRight, size_t& oTop, size_t& oBottom) {
		oLeft =   iCLuster.sx >= _countOfS * _S         ? iCLuster.sx - _countOfS * _S : 0;
		oTop =    iCLuster.sy >= _countOfS * _S         ? iCLuster.sy - _countOfS * _S : 0;
		oRight =  iCLuster.sx +  _countOfS * _S < _imgW ? iCLuster.sx + _countOfS * _S : _imgW - 1;
		oBottom = iCLuster.sy +  _countOfS * _S < _imgH ? iCLuster.sy + _countOfS * _S : _imgH - 1;
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

		Channel ch(_imgW * 3, _imgH);
		ImageRGB im(ch, ch, ch);

		for (size_t i = 0; i < _imgW; ++i) {
			for (size_t j = 0; j < _imgH; ++j) {
				double r, g, b;
				_pImage->Get(i, j, r, g, b);
				im.Set(i, j, r, g, b);
				im.Set(i + _imgW, j, r, g, b);
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
					(i > 0         && aaCI[i][j] != aaCI[i-1][j]) ||
					(i + 1 < _imgW && aaCI[i][j] != aaCI[i+1][j]) ||
					(j > 0         && aaCI[i][j] != aaCI[i][j-1]) ||
					(j + 1 < _imgH && aaCI[i][j] != aaCI[i][j+1])
					) {
					im.Set(i + _imgW, j, 1.0, 0.0, 0.0);
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
							im.Set(px.x + _imgW + _imgW, px.y, r, g, b);
						}

						i++;
					}
				}
			}
		};

		showGrains();
		im.Show();
	}
};


