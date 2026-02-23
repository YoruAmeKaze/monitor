FROM python:3.10-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends \
	gcc \
	python3-dev \
	&& rm -rf /var/lib/apt/lists/*

COPY requirements.txt .
RUN pip install --no-cache-dir --upgrade pip -i https://pypi.tuna.tsinghua.edu.cn/simple && \     
	pip install --no-cache-dir -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple
COPY back/ ./back/
COPY front/ ./front/
RUN mkdir -p /app/back/data
EXPOSE 8000
WORKDIR /app/back
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
